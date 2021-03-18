//
// Created by ashcon on 3/14/21.
//

#ifndef NAG52_SIMULATOR_SHIFT_API_H
#define NAG52_SIMULATOR_SHIFT_API_H


#include <cstdint>
#include "can_frames.h"
#include "nag_iface.h"

#ifdef SIM_MODE
#include <thread>
#endif

#define SHIFT_TIMEOUT 5000 // MILLISECONDS

#define MIN_RPM 1000 // Min allowed RPM
#define MAX_RPM 4500 // red line occurs here

enum class ShiftGear {
    R1,
    R2,
    D1,
    D2,
    D3,
    D4,
    D5
};

enum class ShiftResponse {
    /**
     * Returned in these 2 situations:
     * 1. Cannot shift (up), engine RPM will end up too low!
     * 2. Cannot shift (down), engine RPM will exceed redline!
     */
    RPM_OUT_OF_RANGE,

    /**
     * Cannot accept this shift request - NAG is already
     * in the process of changing gears
     */
    PENDING_SHIFT,

    /**
     * Cannot shift into D2 if gearbox is in EWM is in reverse
     */
     INVALID_SHIFT,

    /**
     * Shift request accepted!
     */
    OK,

    /**
     * Already in gear 5 but want an up shift!?
     */
    GEAR_TOO_HIGH,

    /**
     * Already in gear 1 but want a down shift!?
     */
    GEAR_TOO_LOW
};

class shift_api {
public:
    shift_api(nag_iface* iface);
    shift_api(nag_iface* iface, GS_GIC curr_gear, GS_GZC targ_gear);

    /**
     * Updater function
     */
    void update();

    /**
     * Cancel the current gear shift and go back into
     * the previous gear. This should happen quickly!
     */
    void cancel_shift();

    /**
     * Sets the start gear in Drive
     */
    void set_d_start_gear(ShiftGear d);

    /**
     * Sets the start gear in Reverse
     */
    void set_r_start_gear(ShiftGear r);

    /**
     * Request NAG to up shift with a certain 'firmness'
     * @param firmness Firmness rating (0-255)
     * @return Boolean indicating if shift API accepted the request
     *  and the shift is commencing
     */
    ShiftResponse upshift(uint8_t firmness);

    /**
     * Request NAG to down shift with a certain 'firmness'
     * @param firmness Firmness rating (0-255)
     * @return Boolean indicating if shift API accepted the request
     */
    ShiftResponse downshift(uint8_t firmness);
    // targ_percent is 0-100 (How much lock we want),
    // harshness is 0-255 (How quickly we want it to lock)
    void lock_tcc(uint8_t targ_percent, uint8_t harshness);
    void unlock_tcc();
private:
    void set_max_rpm();
    int max_allowed_rpm = 4000;
    bool is_shifting;
    bool shift_completed;
    bool in_drive = false;

    GS_GIC curr_gear;
    GS_GZC targ_gear;

    GS_GIC curr_gear_disp;
    GS_GZC targ_gear_disp;

    float get_next_gear();
    float get_prev_gear();

    int read_rpm_sensor();
    int read_rpm_sensor_dual();

    EWM_WHC last_ewm_pos;

    ShiftGear start_rev = ShiftGear::R1; // Default start gear
    ShiftGear start_fwd = ShiftGear::D1; // Default start gear

    void go_drive();
    void go_reverse();
    void go_park();
    void go_neutral();

    bool state_change = false;

    GS_GZC map_internal_gear_gzc(ShiftGear x);

    GS_GIC map_internal_gear_gic(ShiftGear x);

    nag_iface* iface;
};

#endif //NAG52_SIMULATOR_SHIFT_API_H
