//
// Created by ashcon on 2/20/21.
//

#ifndef ECU_CAN_SIMULATOR_ABS_ESP_H
#define ECU_CAN_SIMULATOR_ABS_ESP_H

#include <cstdint>
#include "../abstract_ecu.h"



extern short output_shaft_rpm; // Pre differential

class abs_esp : public abstract_ecu {
    void simulate_tick();
public:
    void setup();
};


#endif //ECU_CAN_SIMULATOR_ABS_ESP_H
