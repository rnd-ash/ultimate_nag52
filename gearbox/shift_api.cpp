//
// Created by ashcon on 3/14/21.
//

#include <cstdlib>
#include <cstdio>
#include "shift_api.h"
//#include "helper.h"

#ifdef SIM_MODE
unsigned long millis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
#endif

shift_api::shift_api(nag_iface *iface) {
    this->iface = iface;
    this->gear = NAG_GEAR::D1;
    this->current_gear = GS_GIC::D1;
    this->target_gear = GS_GZC::D1;
    this->tcc_lock = 0;
}

float shift_api::get_gear_ratio(NAG_GEAR a) {
    switch(a) {
        case NAG_GEAR::R1:
            return R1_RAT;
        case NAG_GEAR::R2:
            return R2_RAT;
        case NAG_GEAR::D1:
            return D1_RAT;
        case NAG_GEAR::D2:
            return D2_RAT;
        case NAG_GEAR::D3:
            return D3_RAT;
        case NAG_GEAR::D4:
            return D4_RAT;
        case NAG_GEAR::D5:
            return D5_RAT;
        case NAG_GEAR::P:
        case NAG_GEAR::N:
        default:
            break;
    }
    return 0;
}

GS_GIC shift_api::gear_to_gic(NAG_GEAR a) {
    switch(a) {
        case NAG_GEAR::R1:
            return GS_GIC::R;
        case NAG_GEAR::R2:
            return GS_GIC::R2;
        case NAG_GEAR::D1:
            return GS_GIC::D1;
        case NAG_GEAR::P:
            return GS_GIC::P;
        case NAG_GEAR::N:
            return GS_GIC::N;
        case NAG_GEAR::D2:
            return GS_GIC::D2;
        case NAG_GEAR::D3:
            return GS_GIC::D3;
        case NAG_GEAR::D4:
            return GS_GIC::D4;
        case NAG_GEAR::D5:
            return GS_GIC::D5;
        default:
            break;
    }
    return GS_GIC::SNV;
}

GS_GZC shift_api::gear_to_gzc(NAG_GEAR a) {
    switch(a) {
        case NAG_GEAR::R1:
            return GS_GZC::R;
        case NAG_GEAR::R2:
            return GS_GZC::R2;
        case NAG_GEAR::D1:
            return GS_GZC::D1;
        case NAG_GEAR::P:
            return GS_GZC::P;
        case NAG_GEAR::N:
            return GS_GZC::N;
        case NAG_GEAR::D2:
            return GS_GZC::D2;
        case NAG_GEAR::D3:
            return GS_GZC::D3;
        case NAG_GEAR::D4:
            return GS_GZC::D4;
        case NAG_GEAR::D5:
            return GS_GZC::D5;
        default:
            break;
    }
    return GS_GZC::SNV;
}

// virtual int get_n2_rpm() = 0; // 1, 2, 3, 4, 5
//    virtual int get_n3_rpm() = 0; // 2, 3, 4, R1, R2
//
int shift_api::get_rpm() {
    switch(this->gear) {
        case NAG_GEAR::R1:
            return this->iface->get_n3_rpm();
        case NAG_GEAR::R2:
            return this->iface->get_n3_rpm();
        case NAG_GEAR::D1:
            return this->iface->get_n2_rpm();
        case NAG_GEAR::D2:
            return this->iface->get_n2_rpm();
        case NAG_GEAR::D3:
            return this->iface->get_n2_rpm();
        case NAG_GEAR::D4:
            return this->iface->get_n2_rpm();
        case NAG_GEAR::D5:
            return this->iface->get_n2_rpm();
        case NAG_GEAR::P:
        case NAG_GEAR::N:
        default:
            break;
    }
    return 0;
}

bool shift_api::rpm_sensors_agree() {
    int n3 = this->iface->get_n3_rpm();
    int n2 = this->iface->get_n2_rpm();
    // TODO N2 and N3 comparison
    return true;
}

SHIFT_RESPONSE shift_api::upshift(uint8_t harshness) {
    if (this->is_shifting) {
        return SHIFT_RESPONSE::SHIFT_IN_PROGRESS;
    } else if (this->gear == NAG_GEAR::D5) {
        return SHIFT_RESPONSE::OK; // Todo, 100% lockup TCC 'D6'
    } else if (this->gear <= NAG_GEAR::N) { // N, P, R1, R2
        return SHIFT_RESPONSE::OK; // Illegal, but whatever
    }

    int curr_rpm = ms308.get_NMOT(); // Realtime engine RPM
    this->is_shifting = true;
    switch(this->gear) {
        case NAG_GEAR::D1:
            std::thread(&shift_api::one_to_two, this, false, harshness).detach(); // Spawn shift thread
            break;
        case NAG_GEAR::D2:
            std::thread(&shift_api::two_to_three, this, false, harshness).detach(); // Spawn shift thread
            break;
        case NAG_GEAR::D3:
            std::thread(&shift_api::three_to_four, this, false, harshness).detach(); // Spawn shift thread
            break;
        case NAG_GEAR::D4:
            std::thread(&shift_api::four_to_five, this, false, harshness).detach(); // Spawn shift thread
            break;
    }
    return SHIFT_RESPONSE::OK;
}

SHIFT_RESPONSE shift_api::downshift(uint8_t harshness) {
    if (this->is_shifting) {
        return SHIFT_RESPONSE::SHIFT_IN_PROGRESS;
    } else if (this->gear == NAG_GEAR::D1) {
        return SHIFT_RESPONSE::OK;
    } else if (this->gear <= NAG_GEAR::N) { // N, P, R1, R2
        return SHIFT_RESPONSE::OK; // Illegal, but whatever
    }

    int curr_rpm = ms308.get_NMOT(); // Realtime engine RPM
    int next_rpm = curr_rpm;
    switch(this->gear) {
        case NAG_GEAR::D2:
            this->is_shifting = true;
            this->shift_thread = std::thread(&shift_api::one_to_two, this, true, harshness); // Spawn shift thread
            break;
        case NAG_GEAR::D3:
            this->is_shifting = true;
            this->shift_thread = std::thread(&shift_api::two_to_three, this, true, harshness); // Spawn shift thread
            break;
        case NAG_GEAR::D4:
            this->is_shifting = true;
            this->shift_thread = std::thread(&shift_api::three_to_four, this, true, harshness); // Spawn shift thread
            break;
        case NAG_GEAR::D5:
            this->is_shifting = true;
            this->shift_thread = std::thread(&shift_api::four_to_five, this, true, harshness); // Spawn shift thread
            break;
    }
    return SHIFT_RESPONSE::OK;
}

void shift_api::update() {
    gs418.set_GIC(this->current_gear);
    gs418.set_GZC(this->target_gear);
}

void shift_api::one_to_two(bool reverse, uint8_t harshness) {
    printf("Shift request 1 -> 2\n");
    this->unlock_tcc();
    unsigned long start = millis();

    // Open SPC and Y3
    this->iface->set_one_two(255);
    if (reverse) {
        this->iface->set_spc(harshness/2);
    } else {
        this->iface->set_spc(harshness);
    }
    bool ok = false;
    while (millis() <= start + SHIFT_TIMEOUT) {
        this->target_gear = GS_GZC::D2;
        // TODO Compare RPM
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // TODO, OK and break!
    }
    this->iface->set_one_two(0);
    this->iface->set_spc(0);
    this->is_shifting = false;
    if (ok) {
        printf("Shift 1 -> 2: Shift complete\n");
        this->current_gear = GS_GIC::D2;
    } else {
        printf("Shift 1 -> 2: SHIFT TIMEOUT!\n");
    }
}

void shift_api::lock_tcc(uint8_t harshness) {

}

void shift_api::unlock_tcc() {

}

void shift_api::two_to_three(bool reverse, uint8_t harshness) {

}

void shift_api::three_to_four(bool reverse, uint8_t harshness) {

}

void shift_api::four_to_five(bool reverse, uint8_t harshness) {

}

void shift_api::park_to_reverse(bool reverse) {
    if (reverse) {
        gs418.set_GZC(GS_GZC::P);
        gs418.set_GIC(GS_GIC::P);
    } else {
        gs418.set_GZC(GS_GZC::R);
        gs418.set_GIC(GS_GIC::R);
    }
}

void shift_api::reverse_to_neutral(bool reverse) {
    if (reverse) {
        gs418.set_GZC(GS_GZC::R);
        gs418.set_GIC(GS_GIC::R);
    } else {
        gs418.set_GZC(GS_GZC::N);
        gs418.set_GIC(GS_GIC::N);
    }
}

void shift_api::neutral_to_drive(bool reverse) {
    if (reverse) {
        gs418.set_GZC(GS_GZC::N);
        gs418.set_GIC(GS_GIC::N);
    } else {
        gs418.set_GZC(GS_GZC::D1);
        gs418.set_GIC(GS_GIC::D1);
    }
}
