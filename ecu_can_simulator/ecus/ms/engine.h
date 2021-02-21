//
// Created by ashcon on 2/19/21.
//

#ifndef ECU_CAN_SIMULATOR_ENGINE_H
#define ECU_CAN_SIMULATOR_ENGINE_H

#include <cstdint>
#include "../../canbus/can_sim.h"
#include "../abstract_ecu.h"

// Change these as you wish - These are configured here for my OM646 Engine
#define REDLINE_RPM 4500
#define IDLE_RPM 750

class engine : public abstract_ecu {
public:
    void setup();
private:
    short curr_rpm;
    uint8_t target_gear_lower;
    uint8_t target_gear_upper;
    bool active_aux_heater;
    bool active_downshift;
    bool pedal_press = false;
    double pedal_percentage; // Accelerator position (0-100%)
    bool glow_plug_active;
    bool idle_stable;
    bool vmax_warning;
    bool limp_mode;
    bool kickdown;
    bool speed_limit_enabled;
    CAN_FRAME frame_list[7];

    void simulate_tick();
    void press_pedal();
    void release_pedal();
};


#endif //ECU_CAN_SIMULATOR_ENGINE_H
