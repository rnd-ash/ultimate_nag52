//
// Created by ashcon on 3/6/21.
//

#ifndef ECU_CAN_SIMULATOR_NAG_IFACE_H
#define ECU_CAN_SIMULATOR_NAG_IFACE_H

#include <stdint.h>

// W5A330 - Max torque 330Nm
// W5A580 - Max torque 580Nm

#define W5A330 // Change to W5A580 for the beefier 722.6 gearbox

#ifdef W5A330
#define D1_RAT 3.9319
#define D2_RAT 2.4079
#define D3_RAT 1.4857
#define D4_RAT 1.0000
#define D5_RAT 0.8305
#define R1_RAT -3.1002
#define R2_RAT -1.8986
#elif W5A580
#define D1_RAT 3.5876
#define D2_RAT 2.1862
#define D3_RAT 1.4054
#define D4_RAT 1.0000
#define D5_RAT 0.8314
#define R1_RAT -3.1605
#define R2_RAT -1.9259
#endif

#ifdef SIM_MODE
#include "can_c_enums.h"
#else
#include "../can_frames/can_c_enums.h"
#endif

class nag_iface {
public:
    virtual int get_n2_rpm() = 0; // 1, 2, 3, 4, 5
    virtual int get_n3_rpm() = 0; // 2, 3, 4, R1, R2

    virtual int get_oil_temp() = 0;

    virtual int get_vbatt_mv() = 0; // Battery voltage (mv)

    virtual void set_tcc(uint8_t pwm) = 0; // TCC pressure
    virtual void set_mpc(uint8_t pwm) = 0; // Modulating pressure
    virtual void set_spc(uint8_t pwm) = 0; // Shift pressure

    virtual void set_one_two(uint8_t pwm) = 0; // 1-2 / 4-5
    virtual void set_two_three(uint8_t pwm) = 0; // 2-3
    virtual void set_three_four(uint8_t pwm) = 0; // 3-4

// Set the gearbox mechanic type on CANBUS
#ifdef W5A330
    GS_MECH gearbox_type = GS_MECH::NAG_SMALL;
#elif W5A580
    GS_MECH gearbox_type = GS_MECH::NAG_LARGE;
#endif
};


#endif //ECU_CAN_SIMULATOR_NAG_IFACE_H
