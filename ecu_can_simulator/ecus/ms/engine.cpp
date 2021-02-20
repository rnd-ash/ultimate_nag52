//
// Created by ashcon on 2/19/21.
//

#include "engine.h"

void engine::setup() {
    // Zero out our CAN Data
    ms210.raw = 0x0000484000008000;
    ms212.raw = 0x030C28B928B9A8B9;
    ms268.raw = 0x0000000000000000;
    ms2F3.raw = 0xFF00000000FF0000;
    ms308.raw = 0x000473000068C33A;
    ms312.raw = 0x0768077C0A45074A;
    ms608.raw = 0x6F43062DFA008C00;
    this->idle_stable = true;
    this->curr_rpm = IDLE_RPM; // We are idling to start with
}

void engine::simulate_tick() {
    int oil_temp = 90;
    int coolant_temp = 80;
    ms308.set_NMOT(curr_rpm);
    ms210.set_MSS_AKT(true);
    ms308.set_T_OEL(oil_temp+40);
    ms608.set_T_MOT(coolant_temp + 40);
}