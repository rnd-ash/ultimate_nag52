//
// Created by ashcon on 3/14/21.
//

#include <cstdlib>
#include <cstdio>
#include "shift_api.h"
//#include "helper.h"

shift_api::shift_api(nag_iface* iface) {
    this->iface = iface;
    this->shift_completed = false;
    this->is_shifting = false;
    this->targ_gear = GS_GZC::SNV;
    this->curr_gear = GS_GIC::SNV;

    this->targ_gear_disp = GS_GZC::SNV;
    this->curr_gear_disp = GS_GIC::SNV;
    this->last_ewm_pos = ewm230.get_WHC();
    this->set_max_rpm();
}

shift_api::shift_api(nag_iface* iface, GS_GIC curr_gear, GS_GZC targ_gear) {
    this->iface = iface;
    this->curr_gear = curr_gear;
    this->targ_gear = targ_gear;
    this->curr_gear_disp = curr_gear;
    this->targ_gear_disp = targ_gear;
    this->shift_completed = false;
    this->is_shifting = false;
    this->last_ewm_pos = ewm230.get_WHC();
    this->set_max_rpm();
}

void shift_api::update() {
    EWM_WHC curr_pos = ewm230.get_WHC();
    // For all intensive purposes, +/- position should be D (This selector logic doesn't care)
    if (curr_pos == EWM_WHC::PLUS || curr_pos == EWM_WHC::MINUS) {
        curr_pos = EWM_WHC::D;
    }

    // P -> R
    if (this->last_ewm_pos == EWM_WHC::P && curr_pos == EWM_WHC::P_R) {
        this->go_reverse();
    } // R -> P
    else if (this->last_ewm_pos == EWM_WHC::R && curr_pos == EWM_WHC::P_R) {
        this->go_park();
    } // R -> N
    else if (this->last_ewm_pos == EWM_WHC::R && curr_pos == EWM_WHC::R_N) {
        this->go_neutral();
    } // N -> R
    else if (this->last_ewm_pos == EWM_WHC::N && curr_pos == EWM_WHC::R_N) {
        this->go_reverse();
    } // N -> D
    else if (this->last_ewm_pos == EWM_WHC::N && curr_pos == EWM_WHC::N_D) {
        this->go_drive();
    } // D -> N
    else if (this->last_ewm_pos == EWM_WHC::D && curr_pos == EWM_WHC::N_D) {
        this->go_neutral();
    }

    // Not in any intermediate position
    if (curr_pos != EWM_WHC::N_D && curr_pos != EWM_WHC::P_R) {
        this->last_ewm_pos = curr_pos;
    }
    gs418.set_GZC(this->targ_gear);
    gs418.set_GIC(this->curr_gear);

    if (this->is_shifting) {
        // TODO
    }

}

void shift_api::cancel_shift() {
    if (this->is_shifting && !this->shift_completed) {
        // Cancel the shift! - And go back into our gear
        this->targ_gear_disp = GS_GZC::CANCEL;
        this->curr_gear_disp = GS_GIC::NO_FORCE;

    }
}

ShiftResponse shift_api::upshift(uint8_t firmness) {
    if (this->is_shifting && !this->shift_completed) {
        return ShiftResponse::PENDING_SHIFT;
    }
    int rpm = ms308.get_NMOT();
    if (rpm < MIN_RPM) {
        return ShiftResponse::RPM_OUT_OF_RANGE;
    }

    return ShiftResponse::OK;
}

ShiftResponse shift_api::downshift(uint8_t firmness) {
    if (this->is_shifting && !this->shift_completed) {
        return ShiftResponse::PENDING_SHIFT;
    }
    if (this->curr_gear == GS_GIC::D1 || this->curr_gear == GS_GIC::R) {
        return ShiftResponse::GEAR_TOO_LOW;
    }

    int rpm = ms308.get_NMOT();
    if (rpm >= MAX_RPM) {
        return ShiftResponse::RPM_OUT_OF_RANGE;
    }
    return ShiftResponse::OK;
}

float shift_api::get_next_gear() {
    switch(this->curr_gear) {
        case GS_GIC::D1:
            return D2_RAT;
        case GS_GIC::D2:
            return D3_RAT;
        case GS_GIC::D3:
            return D4_RAT;
        case GS_GIC::D4:
            return D5_RAT;
        case GS_GIC::R2:
            return R1_RAT;
        case GS_GIC::D5:
        case GS_GIC::D6:
        case GS_GIC::D7:
        case GS_GIC::D_CVT:
        case GS_GIC::R_CVT:
        case GS_GIC::R3:
        case GS_GIC::R:
        case GS_GIC::P:
        case GS_GIC::N:
        case GS_GIC::NO_FORCE:
        case GS_GIC::SNV:
        default:
            return 0.0;
    }
}

float shift_api::get_prev_gear() {
    switch(this->curr_gear) {
        case GS_GIC::D2:
            return D1_RAT;
        case GS_GIC::D3:
            return D2_RAT;
        case GS_GIC::D4:
            return D3_RAT;
        case GS_GIC::D5:
            return D4_RAT;
        case GS_GIC::R:
            return R2_RAT;
        case GS_GIC::D1:
        case GS_GIC::D6:
        case GS_GIC::D7:
        case GS_GIC::D_CVT:
        case GS_GIC::R_CVT:
        case GS_GIC::R3:
        case GS_GIC::R2:
        case GS_GIC::P:
        case GS_GIC::N:
        case GS_GIC::NO_FORCE:
        case GS_GIC::SNV:
        default:
            return 0.0;
    }
}

int shift_api::read_rpm_sensor() {
    // REMEMBER:
    // Gears N2 sensor can read: 1, 2, 3, 4, 5
    // Gears N3 sensor can read:    2, 3, 4,   R1, R2
    if ((int)this->curr_gear != (int)this->targ_gear) {
        // Shifting - Check with target gear
        switch (this->targ_gear) {
            case GS_GZC::D1:
            case GS_GZC::D2:
            case GS_GZC::D3:
            case GS_GZC::D4:
            case GS_GZC::D5:
                return iface->get_n2_rpm(); // Read just with N2 during these shifts
            case GS_GZC::R:
            case GS_GZC::R2:
                return iface->get_n3_rpm();
            case GS_GZC::P:
            case GS_GZC::N:
            case GS_GZC::CANCEL:
            case GS_GZC::SNV:
            case GS_GZC::D6:
            case GS_GZC::D7:
            case GS_GZC::D_CVT:
            case GS_GZC::R_CVT:
            case GS_GZC::R3:
            default:
                return 0.0;
        }
    } else {
        // Not shifting - Check with current gear
        switch (this->curr_gear) {
            case GS_GIC::D1:
                return iface->get_n2_rpm();
            case GS_GIC::D2:
            case GS_GIC::D3:
            case GS_GIC::D4:
                // These gears can be read by both N2 and N3. Read with both
                // so we can check if N2 and N3 agree, if not,
                // throw implausible speed sensor error
                return this->read_rpm_sensor_dual();
            case GS_GIC::D5:
                return iface->get_n2_rpm();
            case GS_GIC::R:
            case GS_GIC::R2:
                return iface->get_n3_rpm();
            case GS_GIC::P:
            case GS_GIC::N:
            case GS_GIC::NO_FORCE:
            case GS_GIC::SNV:
            case GS_GIC::D6:
            case GS_GIC::D7:
            case GS_GIC::D_CVT:
            case GS_GIC::R_CVT:
            case GS_GIC::R3:
            default:
                return 0.0;
        }
    }
}

// Max allowed difference between N2 and N3 RPM sensors
#define MAX_ALLOWED_DISAGREE 100

int shift_api::read_rpm_sensor_dual() {
    int n2 = iface->get_n2_rpm();
    int n3 = iface->get_n3_rpm();
    if (std::abs(n2-n3) < MAX_ALLOWED_DISAGREE) {
        return (n2+n3)/2; // Get average RPM of both sensors, they agree
    } else {
        // Sensors disagree!
        return n2; // TODO Handle implausible sensors, calculate which sensor is bad!
    }
}

GS_GZC shift_api::map_internal_gear_gzc(ShiftGear x) {
    switch(x) {
        case ShiftGear::R1:
            return GS_GZC::R;
        case ShiftGear::R2:
            return GS_GZC::R2;
        case ShiftGear::D1:
            return GS_GZC::D1;
        case ShiftGear::D2:
            return GS_GZC::D2;
        case ShiftGear::D3:
            return GS_GZC::D3;
        case ShiftGear::D4:
            return GS_GZC::D4;
        case ShiftGear::D5:
            return GS_GZC::D5;
        default:
            return GS_GZC::SNV;
    }
}

GS_GIC shift_api::map_internal_gear_gic(ShiftGear x) {
    switch(x) {
        case ShiftGear::R1:
            return GS_GIC::R;
        case ShiftGear::R2:
            return GS_GIC::R2;
        case ShiftGear::D1:
            return GS_GIC::D1;
        case ShiftGear::D2:
            return GS_GIC::D2;
        case ShiftGear::D3:
            return GS_GIC::D3;
        case ShiftGear::D4:
            return GS_GIC::D4;
        case ShiftGear::D5:
            return GS_GIC::D5;
        default:
            return GS_GIC::SNV;
    }
}

void shift_api::go_drive() {
    if (this->state_change) {
        return;
    }
    this->state_change = true;
    //LOG_MSG("Changing to drive");
    this->targ_gear = map_internal_gear_gzc(this->start_fwd);
}

void shift_api::go_reverse() {
    if (this->state_change) {
        return;
    }
    this->state_change = true;
    //LOG_MSG("Changing to reverse");
    this->iface->set_three_four(33);
    this->targ_gear = map_internal_gear_gzc(this->start_rev);
}

void shift_api::go_neutral() {
    if (this->state_change) {
        return;
    }
    this->state_change = true;
    //LOG_MSG("Changing to Neutral");
    this->iface->set_three_four(0);
    this->targ_gear = GS_GZC::N;
}

void shift_api::go_park() {
    if (this->state_change) {
        return;
    }
    this->state_change = true;
    //LOG_MSG("Changing to park");
    this->iface->set_three_four(0);
    this->targ_gear = GS_GZC::P;
}

void shift_api::set_max_rpm() {
    // Get engine code
    int eng_code = ms608.get_FCOD_MOT();
    if ((eng_code >= 32 && eng_code <= 46) || eng_code == 96 || eng_code == 99) {
        printf("Diesel engine type! Max RPM 4500\n");
        this->max_allowed_rpm = 4500;
    } else {
        printf("Petrol engine type! Max RPM 6000\n");
        this->max_allowed_rpm = 6000;
    }
}
