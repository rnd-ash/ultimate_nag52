#ifndef ECU_CAN_SIMULATOR_CAR_SIM_H
#define ECU_CAN_SIMULATOR_CAR_SIM_H

#include <cstdint>
#include <string>
#include "esp32_forwarder.h"
#include "../ecus/ecus.h"
#include <thread>
#include <map>

#define TX_FRAME(f) \
{                   \
CAN_FRAME cf;        \
                    \
f.export_frame(&tx.id, &tx.data[0], &tx.dlc); \
this->bcast_frame(&tx); \
}

#define CLEAR_FRAME(f) \
{                    \
f.raw = 0x0000000000000000; \
}

class CAR_SIMULATOR {
public:
    CAR_SIMULATOR(char* port_name);
    void init();
    void init(char* file_name);
    void can_sim_thread();
    void ecu_sim_thread();
    void ecu_sim_thread_replay(std::vector<CAN_FRAME> f);

    void terminate();

    abs_esp* get_abs();
    ewm* get_ewm();
    nag52* get_nag52();
    engine* get_engine();

    std::map<uint32_t, CAN_FRAME> last_frames;
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

extern CAR_SIMULATOR *sim;

// Misc functions to help with printing data!

std::string fmt_frame(CAN_FRAME *f);

#endif //ECU_CAN_SIMULATOR_CAR_SIM_H
