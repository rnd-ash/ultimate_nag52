//
// Created by ashcon on 2/20/21.
//

#ifndef ECU_CAN_SIMULATOR_ABS_ESP_H
#define ECU_CAN_SIMULATOR_ABS_ESP_H

#include <cstdint>
#include "../abstract_ecu.h"
#include "../extern_frames.h"

#define WHEEL_DIAMETER =

class abs_esp : public abstract_ecu {

    void simulate_tick();

public:
    void setup();
};


#endif //ECU_CAN_SIMULATOR_ABS_ESP_H
