//
// Created by ashcon on 2/20/21.
//

#include <cstdio>
#include "ewm.h"

void ewm::setup() {
    ewm230.raw = 0x0000000000000000;
    this->drive_prog_btn = false;
    this->selector_position = EWM_WHC::P; // Park to start with
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
        printf("Selector position: %d\n", this->selector_position);
        if (selector_move_dir == MOVE_DIR::UP) {
            switch(this->selector_position) {
                case EWM_WHC::D:
                    this->selector_position = EWM_WHC::N_D;
                    break;
                case EWM_WHC::N:
                    this->selector_position = EWM_WHC::R_N;
                    break;
                case EWM_WHC::R:
                    this->selector_position = EWM_WHC::P_R;
                    break;
                case EWM_WHC::N_D:
                    this->selector_position = EWM_WHC::N;
                    break;
                case EWM_WHC::R_N:
                    this->selector_position = EWM_WHC::R;
                    break;
                case EWM_WHC::P_R:
                    this->selector_position = EWM_WHC::P;
                    break;
                case EWM_WHC::P:
                case EWM_WHC::PLUS:
                case EWM_WHC::MINUS:
                case EWM_WHC::SNV: // 0xFF
                    break;
            }
        } else if (selector_move_dir == MOVE_DIR::DOWN) {
            switch(this->selector_position) {
                case EWM_WHC::N:
                    this->selector_position = EWM_WHC::N_D;
                    break;
                case EWM_WHC::R:
                    this->selector_position = EWM_WHC::R_N;
                    break;
                case EWM_WHC::P:
                    this->selector_position = EWM_WHC::P_R;
                    break;
                case EWM_WHC::N_D:
                    this->selector_position = EWM_WHC::D;
                    break;
                case EWM_WHC::R_N:
                    this->selector_position = EWM_WHC::N;
                    break;
                case EWM_WHC::P_R:
                    this->selector_position = EWM_WHC::R;
                    break;
                case EWM_WHC::D:
                case EWM_WHC::PLUS:
                case EWM_WHC::MINUS:
                case EWM_WHC::SNV:
                    break;
            }
        } else if (selector_move_dir == MOVE_DIR::RIGHT) {
            switch(this->selector_position) {
                case EWM_WHC::D:
                    this->selector_position = EWM_WHC::PLUS;
                    break;
                case EWM_WHC::MINUS:
                    this->selector_position = EWM_WHC::D;
                    break;
                case EWM_WHC::N:
                case EWM_WHC::R:
                case EWM_WHC::P:
                case EWM_WHC::PLUS:
                case EWM_WHC::N_D:
                case EWM_WHC::R_N:
                case EWM_WHC::P_R:
                case EWM_WHC::SNV:
                    break;
            }
        } else if (selector_move_dir == MOVE_DIR::LEFT) {
            switch(this->selector_position) {
                case EWM_WHC::D:
                    this->selector_position = EWM_WHC::MINUS;
                    break;
                case EWM_WHC::PLUS:
                    this->selector_position = EWM_WHC::D;
                    break;
                case EWM_WHC::MINUS:
                case EWM_WHC::N:
                case EWM_WHC::R:
                case EWM_WHC::P:
                case EWM_WHC::N_D:
                case EWM_WHC::R_N:
                case EWM_WHC::P_R:
                case EWM_WHC::SNV:
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
