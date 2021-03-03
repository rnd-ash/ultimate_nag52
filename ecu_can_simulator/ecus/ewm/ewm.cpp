//
// Created by ashcon on 2/20/21.
//

#include "ewm.h"

void ewm::setup() {
    ewm230.raw = 0x0000000000000000;
    this->drive_prog_btn = false;
    this->selector_position = GS_P; // Park to start with
}

void ewm::simulate_tick() {
    ewm230.set_WHC(this->selector_position);
    ewm230.set_FPT(this->drive_prog_btn);

    ewm230.set_KD(false); // Always - W203 EWM does not monitor kickdown
    ewm230.set_W_S(false); // Always - W203 EWM does not monitor this button, only if it is pressed


    this->sim_ticks ++;
    if (this->sim_ticks >= 50) {
        this->sim_ticks = 0;
        // Move the selector position based on the force being applied to it by the user
        if (selector_move_dir == MOVE_DIR::UP) {
            switch(this->selector_position) {
                case GS_D:
                    this->selector_position = WHC::GS_N_D;
                    break;
                case GS_N:
                    this->selector_position = WHC::GS_R_N;
                    break;
                case GS_R:
                    this->selector_position = WHC::GS_P_R;
                    break;
                case GS_N_D:
                    this->selector_position = WHC::GS_D;
                    break;
                case GS_R_N:
                    this->selector_position = WHC::GS_R;
                    break;
                case GS_P_R:
                    this->selector_position = WHC::GS_P;
                    break;
                case GS_P:
                case GS_PLUS:
                case GS_MINUS:
                case GS_SNV:
                    break;
            }
        } else if (selector_move_dir == MOVE_DIR::DOWN) {
            switch(this->selector_position) {
                case GS_N:
                    this->selector_position = WHC::GS_N_D;
                    break;
                case GS_R:
                    this->selector_position = WHC::GS_R_N;
                    break;
                case GS_P:
                    this->selector_position = WHC::GS_P_R;
                    break;
                case GS_N_D:
                    this->selector_position = WHC::GS_D;
                    break;
                case GS_R_N:
                    this->selector_position = WHC::GS_N;
                    break;
                case GS_P_R:
                    this->selector_position = WHC::GS_R;
                    break;
                case GS_D:
                case GS_PLUS:
                case GS_MINUS:
                case GS_SNV:
                    break;
            }
        } else if (selector_move_dir == MOVE_DIR::RIGHT) {
            switch(this->selector_position) {
                case GS_D:
                    this->selector_position = GS_PLUS;
                    break;
                case GS_MINUS:
                    this->selector_position = GS_D;
                    break;
                case GS_N:
                case GS_R:
                case GS_P:
                case GS_PLUS:
                case GS_N_D:
                case GS_R_N:
                case GS_P_R:
                case GS_SNV:
                    break;
            }
        } else if (selector_move_dir == MOVE_DIR::LEFT) {
            switch(this->selector_position) {
                case GS_D:
                    this->selector_position = GS_MINUS;
                    break;
                case GS_PLUS:
                    this->selector_position = GS_D;
                    break;
                case GS_MINUS:
                case GS_N:
                case GS_R:
                case GS_P:
                case GS_N_D:
                case GS_R_N:
                case GS_P_R:
                case GS_SNV:
                    break;
            }
        }
    }

}

void ewm::apply_force(MOVE_DIR dir) {
    this->selector_move_dir = dir;
}

void ewm::press_btn() {
    this->drive_prog_btn = true;
}

void ewm::release_btn() {
    this->drive_prog_btn = false;
}
