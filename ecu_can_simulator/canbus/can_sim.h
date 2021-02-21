#ifndef ECU_CAN_SIMULATOR_CAN_SIM_H
#define ECU_CAN_SIMULATOR_CAN_SIM_H

#include <cstdint>
#include <string>
#include "../ecus/abstract_ecu.h"
#include "esp32_forwarder.h"
#include <thread>

// Engine frames
#include <MS_210.h>
#include <MS_212.h>
#include <MS_268.h>
#include <MS_2F3.h>
#include <MS_308.h>
#include <MS_312.h>
#include <MS_608.h>

// GS frames
#include <GS_218.h>
#include <GS_338.h>
#include <GS_418.h>

// ABS/ESP frames
#include <BS_200.h>
#include <BS_208.h>
#include <BS_270.h>
#include <BS_300.h>
#include <BS_328.h>

// Engine frames
extern MS_210 ms210;
extern MS_212 ms212;
extern MS_268 ms268;
extern MS_2F3 ms2F3;
extern MS_308 ms308;
extern MS_308 ms312;
extern MS_608 ms608;

// GS frames
extern GS_218 gs218;
extern GS_338 gs338;
extern GS_418 gs418;

// ABS/ESP frames
extern BS_200 bs200;
extern BS_208 bs208;
extern BS_270 bs270;
extern BS_300 bs300;
extern BS_328 bs328;


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

class CAN_SIMULATOR {
public:
    CAN_SIMULATOR(char* port_name);
    void init(std::vector<abstract_ecu*> ecu_list);
    void can_sim_thread();
    void ecu_sim_thread();
    void terminate();
private:
    std::vector<abstract_ecu*> ecus;
    bool send_to_esp = false;
    esp32_forwarder esp32;
    bool thread_exec;
    void bcast_frame(CAN_FRAME *f);
    std::thread sim_thread;
    std::thread can_thread;
};

// Misc functions to help with printing data!

std::string fmt_frame(CAN_FRAME *f);

#endif //ECU_CAN_SIMULATOR_CAN_SIM_H
