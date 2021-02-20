//
// Created by ashcon on 2/20/21.
//

#ifndef ECU_CAN_SIMULATOR_EWM_H
#define ECU_CAN_SIMULATOR_EWM_H
#include "../abstract_ecu.h"
#include "EWM_230.h"

enum MOVE_DIR {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

/**
 *  EWM Module looks like this
 *
 *   --------------
 *   |s/c         |
 *   |      P     |
 *   |      R     |
 *   |      N     |
 *   |    - D +   |
 *   --------------
 */
class ewm : abstract_ecu {
public:
    void setup();
    void apply_force(MOVE_DIR dir);
    void press_btn();
    void release_btn();
private:
    int sim_ticks = 0;
    EWM_230 ewm230;
    WHC selector_position = WHC::GS_P;
    bool drive_prog_btn = false;
    void simulate_tick();
    MOVE_DIR selector_move_dir;
};

#endif //ECU_CAN_SIMULATOR_EWM_H
