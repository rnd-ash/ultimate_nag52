//
// Created by ashcon on 3/6/21.
//

#ifndef ECU_CAN_SIMULATOR_NAG_IFACE_H
#define ECU_CAN_SIMULATOR_NAG_IFACE_H

#include <stdint.h>

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
};


#endif //ECU_CAN_SIMULATOR_NAG_IFACE_H
