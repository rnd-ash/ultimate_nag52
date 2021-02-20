//
// Created by ashcon on 2/19/21.
//

#ifndef ECU_CAN_SIMULATOR_ABSTRACT_ECU_H
#define ECU_CAN_SIMULATOR_ABSTRACT_ECU_H


#include <vector>

class abstract_ecu {
public:
    void setup();
    std::vector<int> listen_addresses; // What should the ECU listen to on CANBUS?
    int bcast_frequency; // How many times per second should this ECU send all its frames?
protected:

};


#endif //ECU_CAN_SIMULATOR_ABSTRACT_ECU_H
