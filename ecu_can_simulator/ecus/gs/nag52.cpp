//
// Created by ashcon on 2/20/21.
//

#include <cstdio>
#include "nag52.h"

void nag52::setup() {
    gs418.raw = 0x504D7404DD00C000;
    gs218.raw = 0x0000DD4923003060;
    gs338.raw = 0x0000000000000000;
    this->limp_mode = false;
    this->handle_press = false;
    this->prog = DriveProgramMode::Sport;
}

bool x = false;
void nag52::simulate_tick() {
    gs418.set_ALL_WHEEL(false);
    gs418.set_CVT(false);
    gs418.set_T_GET(80+40); // 80C
    gs218.set_GSP_OK(!this->limp_mode);
    gs218.set_GS_NOTL(this->limp_mode);

    switch(ewm230.get_WHC()) {
        case GS_P:
            gs418.set_WHST(0);
            gs418.set_FSC(80);
            break;
        case GS_D:
        case GS_PLUS:
        case GS_MINUS:
            gs418.set_WHST(4);
            gs418.set_FSC(68); // TODO - Range restrict mode
            break;
        case GS_N:
            gs418.set_WHST(2);
            gs418.set_FSC(78);
            break;
        case GS_R:
            gs418.set_WHST(1);
            gs418.set_FSC(82);
            break;
        case GS_SNV:
            gs418.set_WHST(7);
            gs418.set_FSC(255);
            break;
        default:
            break;
    }

    // IC only listens to FSC value (Not sure about the rest of the car)
    switch(this->prog) {
        case DriveProgramMode::Sport:
            gs418.set_FPC(DrivingProgram::S);
            break;
        case DriveProgramMode::Comfort:
            gs418.set_FPC(DrivingProgram::C);
            break;
        case DriveProgramMode::Agility:
            gs418.set_FPC(DrivingProgram::A);
            break;
        case DriveProgramMode::Manual:
            gs418.set_FPC(DrivingProgram::M);
            break;
        case DriveProgramMode::Fail:
            gs418.set_FPC(DrivingProgram::F);
            break;
        default:
            break;
    }

    gs418.set_FPC(90);
    if (ewm230.get_FPT()) {
        this->handle_btn_press();
    } else {
        this->handle_press = false;
    }


#ifdef W5A330
    gs418.set_MECH(GearVariant::NAG2_KLEIN2);
#else
    gs418.set_MECH(GearVariant::NAG2_GROSS2);
#endif



}

// Returns the current gear ratio (Actual) from the input to output shaft
// of the gearbox
float nag52::get_current_ratio() {
    return this->curr_gear_ratio;
}

void nag52::handle_btn_press() {
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
}
