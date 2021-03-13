//
// Created by ashcon on 2/19/21.
//

#ifndef ECU_CAN_SIMULATOR_ABSTRACT_ECU_H
#define ECU_CAN_SIMULATOR_ABSTRACT_ECU_H


#include <vector>
#include "can_frame.h"

struct frame_list {
    uint8_t count;
    CAN_FRAME* ptr;
};

class abstract_ecu {
public:
    virtual void setup() = 0;

    /**
     * Called every 10ms (Update current ECUs state!)
     */
    virtual void simulate_tick() = 0;
protected:
    frame_list* frames = nullptr;
};


#endif //ECU_CAN_SIMULATOR_ABSTRACT_ECU_H
