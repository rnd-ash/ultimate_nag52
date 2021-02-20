#include <iostream>
#include "canbus/can_sim.h"
#include "ecus/ms/engine.h"
#include "ecus/bs/abs_esp.h"
#include "ecus/gs/nag52.h"
#include "MS_308.h"
#include "MS_608.h"
#include <thread>
int main() {
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
    std::this_thread::sleep_for(std::chrono::seconds(10));
    sim.terminate();
    return 0;
}
