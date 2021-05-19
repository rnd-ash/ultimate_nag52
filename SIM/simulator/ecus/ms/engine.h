//
// Created by ashcon on 2/19/21.
//

#ifndef ECU_CAN_SIMULATOR_ENGINE_H
#define ECU_CAN_SIMULATOR_ENGINE_H



#include <cstdint>
#include "../abstract_ecu.h"
#include "can_frames.h"

// Change these as you wish - These are configured here for my OM646 Engine
#define REDLINE_RPM 4500
#define IDLE_RPM 750

class engine : public abstract_ecu {
public:
    void setup();
    void simulate_tick();
    void press_pedal();
    void release_pedal();
    void force_set_rpm(short rpm, float ratio);
private:
    float d_rpm;
    float d_rpm_drag = 1;
    float curr_rpm;
    float fuel_usage;
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
};


#endif //ECU_CAN_SIMULATOR_ENGINE_H
