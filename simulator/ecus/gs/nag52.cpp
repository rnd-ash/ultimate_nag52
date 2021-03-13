//
// Created by ashcon on 2/20/21.
//

#include <cstdio>
#include "nag52.h"

#define UPSHIFT_ON_REDLINE // Comment out for redline bounding in drive and manual mode!
#define MANUAL_2_START // Comment out to prevent optionally starting in 2nd in manual mode!

void nag52::setup() {
    this->iface = virtual_nag_iface();
    this->sim = Gearbox();
    this->sim.startup(&this->iface);
}

void nag52::simulate_tick() {
    this->sim.loop();
}

void nag52::handle_btn_press() {
    /*
    if (this->limp_mode) { // Ignore this request in limp!
        this->prog = DriveProgramMode::Fail;
        return;
    }
    if (!this->handle_press) {
        // S -> C -> A -> M -> S
        switch (prog) {
            case DriveProgramMode::Sport:
                this->prog = DriveProgramMode::Comfort;
                break;
            case DriveProgramMode::Comfort:
                this->prog = DriveProgramMode::Winter;
                break;
            case DriveProgramMode::Winter:
                this->prog = DriveProgramMode::Agility;
                break;
            case DriveProgramMode::Agility:
                this->prog = DriveProgramMode::Manual;
                break;
            case DriveProgramMode::Manual:
                this->prog = DriveProgramMode::Sport;
                break;
            default:
                break;
        }
        this->handle_press = true;
    }
     */
}

// Assumes shift has just 'completed' engine RPM must be set
int nag52::force_engine_rpm() {
    /*
    switch (this->curr_fwd_gear) {
        case 1:
            return output_shaft_rpm * D1_RAT;
        case 2:
            return output_shaft_rpm * D2_RAT;
        case 3:
            return output_shaft_rpm * D3_RAT;
        case 4:
            return output_shaft_rpm * D4_RAT;
        case 5:
            return output_shaft_rpm * D5_RAT;
        default:
            return 0;
    }
    */
    return 0;
}

void nag52::request_upshift_release() {
    this->up_pressed = false;
}

void nag52::request_downshift_release() {
    this->down_pressed = false;
}

// Manual up/downshift logic is a bit different
// We have to check if the NEXT gear is within range before switching
void nag52::request_upshift_press() {

}

void nag52::request_downshift_press() {

}

int virtual_nag_iface::get_n2_rpm() {
    int engine_rpm = ms308.get_NMOT();
    int output_rpm = 0;
    switch(this->virtual_gear) {
        case V_GEAR::D_1:
            output_rpm = engine_rpm / D1_RAT;
            break;
        case V_GEAR::D_2:
            output_rpm = engine_rpm / D2_RAT;
            break;
        case V_GEAR::D_3:
            output_rpm = engine_rpm / D3_RAT;
            break;
        case V_GEAR::D_4:
            output_rpm = engine_rpm / D4_RAT;
            break;
        case V_GEAR::D_5:
            output_rpm = engine_rpm / D5_RAT;
            break;
        case V_GEAR::PARK:
        case V_GEAR::NEUTRAL:
        case V_GEAR::REV_1:
        case V_GEAR::REV_2:
        default:
            break;

    }
    return output_rpm;
}

int virtual_nag_iface::get_n3_rpm() {
    int engine_rpm = ms308.get_NMOT();
    int output_rpm = 0;
    switch(this->virtual_gear) {
        case V_GEAR::D_2:
            output_rpm = D2_RAT * engine_rpm;
            break;
        case V_GEAR::D_3:
            output_rpm = D3_RAT * engine_rpm;
            break;
        case V_GEAR::D_4:
            output_rpm = D4_RAT * engine_rpm;
            break;
        case V_GEAR::REV_1:
            output_rpm = R1_RAT * engine_rpm;
            break;
        case V_GEAR::REV_2:
            output_rpm = D2_RAT * engine_rpm;
            break;
        case V_GEAR::D_1:
        case V_GEAR::D_5:
        case V_GEAR::PARK:
        case V_GEAR::NEUTRAL:
        default:
            break;
    }
    return output_rpm;
}

int virtual_nag_iface::get_oil_temp() {
    return 80;
}

int virtual_nag_iface::get_vbatt_mv() {
    return 14300; // 14.3V
}

void virtual_nag_iface::set_tcc(uint8_t pwm) {

}

void virtual_nag_iface::set_mpc(uint8_t pwm) {
    // Ignore
}

void virtual_nag_iface::set_spc(uint8_t pwm) {
    // Ignore
}

void virtual_nag_iface::set_three_four(uint8_t pwm) {
    if (this->virtual_gear == V_GEAR::D_3) {
        this->virtual_gear = V_GEAR::D_4;
    } else if (this->virtual_gear == V_GEAR::D_4) {
        this->virtual_gear = V_GEAR::D_3;
    }
}

void virtual_nag_iface::set_two_three(uint8_t pwm) {
    if (this->virtual_gear == V_GEAR::D_2) {
        this->virtual_gear = V_GEAR::D_3;
    } else if (this->virtual_gear == V_GEAR::D_3) {
        this->virtual_gear = V_GEAR::D_2;
    }
}

void virtual_nag_iface::set_one_two(uint8_t pwm) {
    if (this->virtual_gear == V_GEAR::D_1) {
        this->virtual_gear = V_GEAR::D_2;
    } else if (this->virtual_gear == V_GEAR::D_2) {
        this->virtual_gear = V_GEAR::D_1;
    } else if (this->virtual_gear == V_GEAR::D_4) {
        this->virtual_gear = V_GEAR::D_5;
    } else if (this->virtual_gear == V_GEAR::D_5) {
        this->virtual_gear = V_GEAR::D_4;
    }
}

void virtual_nag_iface::set_ewm_position(int g) {
    if (g == 0) {
        this->virtual_gear = V_GEAR::PARK;
    } else if (g == 1) {
        this->virtual_gear = V_GEAR::REV_1;
    } else if (g == 2) {
        this->virtual_gear = V_GEAR::NEUTRAL;
    } else if (g == 3) {
        if (
                this->virtual_gear == V_GEAR::PARK
                || this->virtual_gear == V_GEAR::REV_1
                || this->virtual_gear == V_GEAR::REV_2
                || this->virtual_gear == V_GEAR::NEUTRAL
                ) {
            this->virtual_gear = V_GEAR::D_1;
        }
    }
}
