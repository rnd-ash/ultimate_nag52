//
// Created by ashcon on 2/19/21.
//

#include <cstdio>
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
    this->pedal_press = false;
    this->pedal_percentage = 0.0;
    this->d_rpm = 0;
}

void engine::simulate_tick() {



    int oil_temp = 90;
    int coolant_temp = 80;
    //ms308.set_NMOT(curr_rpm);
    ms308.set_VGL_KL(false);
    ms210.set_MSS_AKT(true);
    ms308.set_T_OEL(oil_temp+40);
    ms608.set_T_MOT(coolant_temp + 40);
    ms308.set_NMOT((short)this->curr_rpm);
    ms308.set_UEHITZ(oil_temp > 120);

    this->curr_rpm += this->d_rpm;

    if (this->curr_rpm < IDLE_RPM) {
        this->d_rpm = IDLE_RPM - this->curr_rpm;
    }

    if (this->pedal_press && this->curr_rpm < REDLINE_RPM && this->d_rpm < 15) {
        this->d_rpm += 0.05 * this->pedal_percentage;
    } else if (this->curr_rpm >= REDLINE_RPM && d_rpm > 0) {
        this->d_rpm -= 0.5;
    } else if (this->curr_rpm > IDLE_RPM && d_rpm > -10) {
        this->d_rpm -= 0.2;
    }
    printf("CURR RPM: %.1f\n", this->curr_rpm);


}

void engine::press_pedal() {
    this->pedal_press = true;
    if(this->pedal_percentage < 100) {
        this->pedal_percentage++;
    }
}

void engine::release_pedal() {
    this->pedal_press = false;
    this->pedal_percentage = 0;
}