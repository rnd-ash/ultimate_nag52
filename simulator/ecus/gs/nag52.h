//
// Created by ashcon on 2/20/21.
//

#ifndef ECU_CAN_SIMULATOR_NAG52_H
#define ECU_CAN_SIMULATOR_NAG52_H

#include "../abstract_ecu.h"
#include "../../../nag_iface.h"
#include "../../../gearbox_brain.h"



enum V_GEAR {
    PARK,
    REV_1,
    REV_2,
    NEUTRAL,
    D_1,
    D_2,
    D_3,
    D_4,
    D_5
};

class virtual_nag_iface : public nag_iface {
public:
    int get_n2_rpm() override; // 1, 2, 3, 4, 5
    int get_n3_rpm() override; // 2, 3, 4, R1, R2

    int get_oil_temp() override;

    int get_vbatt_mv() override; // Battery voltage (mv)

    void set_tcc(uint8_t pwm) override; // TCC pressure
    void set_mpc(uint8_t pwm) override; // Modulating pressure
    void set_spc(uint8_t pwm) override; // Shift pressure

    void set_one_two(uint8_t pwm) override; // 1-2 / 4-5
    void set_two_three(uint8_t pwm) override; // 2-3
    void set_three_four(uint8_t pwm) override; // 3-4

    void set_ewm_position(int g);
private:
    V_GEAR virtual_gear = V_GEAR::PARK;
};

/*
 * http://ftp.hamsk.ru/auto/W210/AKPP%20Mercedes-Manual.pdf
 *
 * NAG52 controls 5 solenoids:
 * 1. TCC solenoid (PWM)
 * 2. MPC solenoid (PWM)
 * 3. SPC solenoid (PWM)
 * 4. 1-2 & 4-5 solenoid (ON/OFF)
 * 5. 2-3 solenoid (ON/OFF)
 * 6. 3-4 solenoid (ON/OFF)
 *
 * MPC's job is to keep a constant line pressure regardless of the pump speed based on engine RPM
 * SPC's job is to set the pressure for changing gears. Only PWM during gear shifts, else it is off
 * TCC's job is to lock up the torque converter
 * other solenoids are only active during gear shifts.
 *
 * # Some points:
 * 1-2/4-5 solenoid is pulsed during cranking of the engine
 * 3-4 solenoid is pulsed while in park and when EWM module is moving (Garage shifting)
 * MPC pulses at 40% when in park or neutral
 * SPC pulses at 33% in park or neutral
 */

class nag52 : public abstract_ecu {
public:
    void setup();
    void simulate_tick();

    void request_upshift_press();
    void request_downshift_press();

    void request_upshift_release();
    void request_downshift_release();

private:
    bool up_pressed = false;
    bool down_pressed = false;
    virtual_nag_iface iface;
    Gearbox sim;

    int last_rpm = 0;
    int force_engine_rpm();
    void handle_btn_press();
    bool handle_press = false;
};





#endif //ECU_CAN_SIMULATOR_NAG52_H
