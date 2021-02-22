#include <iostream>
#include "canbus/can_sim.h"
#include "ecus/ms/engine.h"
#include "ecus/bs/abs_esp.h"
#include "ecus/gs/nag52.h"
#include <thread>
#include "v_kombi/virtual_kombi.h"
int main(int argc, char *argv[]) {
    CAN_SIMULATOR sim((char*)"/dev/ttyUSB0");
    std::vector<abstract_ecu*> ecu_list;
    engine eng = engine();
    abs_esp brake_monitor = abs_esp();
    nag52 nag = nag52();
    eng.setup();
    brake_monitor.setup();
    nag.setup();
    ecu_list.push_back(&eng);
    ecu_list.push_back(&brake_monitor);
    ecu_list.push_back(&nag);
    sim.init(ecu_list);
    virtual_kombi(&sim).loop();
    return 0;
}
