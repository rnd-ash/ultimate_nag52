#ifndef ECU_CAN_SIMULATOR_CAR_SIM_H
#define ECU_CAN_SIMULATOR_CAR_SIM_H

#include <cstdint>
#include <string>
#include "../ecus/abstract_ecu.h"
#include "esp32_forwarder.h"
#include "../ecus/extern_frames.h"

#include <thread>



#define TX_FRAME(f) \
{                    \
f.export_frame(tx); \
this->bcast_frame(&tx); \
std::this_thread::sleep_for(std::chrono::microseconds(100)); \
}

#define CLEAR_FRAME(f) \
{                    \
f.raw = 0x0000000000000000; \
}

class CAR_SIMULATOR {
public:
    CAR_SIMULATOR(char* port_name);
    void init();
    void can_sim_thread();
    void ecu_sim_thread();
    void terminate();

    abs_esp* get_abs();
    ewm* get_ewm();
    nag52* get_nag52();
    engine* get_engine();
private:
    std::vector<abstract_ecu*> ecus;
    abs_esp* esp_ecu;
    ewm* ewm_ecu;
    nag52* nag52_ecu;
    engine* eng_ecu;

    bool send_to_esp = false;
    esp32_forwarder esp32;
    bool thread_exec;
    void bcast_frame(CAN_FRAME *f);
    std::thread sim_thread;
    std::thread can_thread;
};

// Misc functions to help with printing data!

std::string fmt_frame(CAN_FRAME *f);

#endif //ECU_CAN_SIMULATOR_CAR_SIM_H
