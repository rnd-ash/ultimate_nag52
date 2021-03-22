//
// Created by ashcon on 3/14/21.
//

#ifndef NAG52_SIMULATOR_SHIFT_API_H_
#define NAG52_SIMULATOR_SHIFT_API_H_


#include <cstdint>
#include "can_frames.h"
#include "nag_iface.h"

#ifdef SIM_MODE
#include <thread>
#include <chrono>
unsigned long millis();
#endif



#define SHIFT_TIMEOUT 5000 // MILLISECONDS
#define MIN_RPM 1000 // Min allowed RPM


enum class SHIFT_RESPONSE {
    // Shift OK - Has started
    OK,

    // Currently shifting gears, cannot change
    SHIFT_IN_PROGRESS,

    // Transmission is in limp mode, cannot shift gears
    LIMP_MODE,
};

class shift_api {
public:
    void update(); // Set CAN data
    shift_api(nag_iface* iface);
    SHIFT_RESPONSE upshift(uint8_t harshness);
    SHIFT_RESPONSE downshift(uint8_t harshness);
    void lock_tcc(uint8_t harshness);
    void unlock_tcc(); // Is this really needed?

    // Called by shifter threads!
    void one_to_two(bool reverse, uint8_t harshness);
    void two_to_three(bool reverse, uint8_t harshness);
    void three_to_four(bool reverse, uint8_t harshness);
    void four_to_five(bool reverse, uint8_t harshness);

    void park_to_reverse(bool reverse);
    void reverse_to_neutral(bool reverse);
    void neutral_to_drive(bool reverse);
private:
    GS_GIC gear_to_gic(NAG_GEAR a);
    GS_GZC gear_to_gzc(NAG_GEAR a);
    float get_gear_ratio(NAG_GEAR a);

    uint8_t tcc_lock{};

    bool is_shifting = false;
    int redline_rpm = 4500;
    nag_iface* iface;

    NAG_GEAR gear;

    GS_GIC current_gear;
    GS_GZC target_gear;

    std::thread shift_thread;

    // Returns NAG RPM (From N2/N3 sensors)
    int get_rpm();
    bool rpm_sensors_agree();

    // Returns difference between Motor RPM and NAG RPM
    int get_slip_rpm();

};

#endif //NAG52_SIMULATOR_SHIFT_API_H
