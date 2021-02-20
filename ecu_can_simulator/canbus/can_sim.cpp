//
// Created by ashcon on 2/20/21.
//

#include "can_sim.h"
#include "can_frame.h"
#include "esp32_forwarder.h"

#include <utility>
#include <cstring>

#include "EZS_A5.h"

// Engine frames
MS_210 ms210;
MS_212 ms212;
MS_268 ms268;
MS_2F3 ms2F3;
MS_308 ms308;
MS_308 ms312;
MS_608 ms608;

// GS frames
GS_218 gs218;
GS_338 gs338;
GS_418 gs418;

// ABS/ESP frames
BS_200 bs200;
BS_208 bs208;
BS_270 bs270;
BS_300 bs300;
BS_328 bs328;
EZS_A5 ezsa5; // For variant testing!

std::string fmt_frame(CAN_FRAME *f) {
    char buf[150] = {0x00};
    int pos = sprintf(buf, "0x%04X - [", f->id);
    for (int i = 0; i < f->length; i++) {
        pos += sprintf(buf+pos, "%02X ", f->data.bytes[i]);
    }
    buf[pos-1] = ']';
    return std::string(buf);
}

void CAN_SIMULATOR::init(std::vector<abstract_ecu*> ecu_list) {
    this->ecus = std::move(ecu_list);
    printf("Init with %lu ecus!\n", ecus.size());
    this->thread_exec = true;
    this->can_thread = std::thread(&CAN_SIMULATOR::can_sim_thread, this);
    this->sim_thread = std::thread(&CAN_SIMULATOR::ecu_sim_thread, this);
}

void CAN_SIMULATOR::can_sim_thread() {
    printf("CAN sender thread starting!\n");
    CAN_FRAME tx;
    ezsa5.raw = 0x020024062D18181A;

    // Testing variant coding options from EZS_A5 frame
    ezsa5.set_LL_RL(1); // Left hand drive, 2 is RHD
    ezsa5.set_LAND(6); // Set to gulf state.
    ezsa5.set_KSG_VH(true); // Manual transmission - Yes
    ezsa5.set_NAG_VH(false); // Auto transmission - No
    ezsa5.set_ART_VH(true); // Art available
    ezsa5.set_PTS_VH(true); // Enable parktronics
    ezsa5.set_KG_VH(true); // Enable keyless go system

    int ticks = 0;
    while(this->thread_exec) {
        ticks++;
        if (ticks >= 10) {
            ticks = 0;
            // Send EZS frame to keep IC thinking engine is running
            TX_FRAME(ezsa5)
        }
        // Send frames in order of ID
        TX_FRAME(ms210)
        TX_FRAME(ms212)
        TX_FRAME(ms268)
        TX_FRAME(ms2F3)
        TX_FRAME(ms308)
        //TX_FRAME(ms312)
        TX_FRAME(ms608)
        TX_FRAME(gs218)
        TX_FRAME(gs338)
        TX_FRAME(gs418)
        TX_FRAME(bs200)
        TX_FRAME(bs208)
        TX_FRAME(bs270)
        TX_FRAME(bs300)
        TX_FRAME(bs328)
    }
}

void CAN_SIMULATOR::ecu_sim_thread() {
    printf("ECU Simulator thread starting!\n");
    while(this->thread_exec) {
        for (auto ecu: this->ecus) {
            ecu->simulate_tick();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void CAN_SIMULATOR::terminate() {
    this->thread_exec = false;
    this->can_thread.join();
    this->sim_thread.join();
    if (this->send_to_esp) {
        this->esp32.close_port();
    }
}

void CAN_SIMULATOR::bcast_frame(CAN_FRAME *f) {
    if (this->send_to_esp) {
        this->esp32.send_frame(f);
    }
}

CAN_SIMULATOR::CAN_SIMULATOR(char *port_name) {
    if (port_name != nullptr) {
        this->esp32 = esp32_forwarder(port_name);
        if (this->esp32.is_port_open()) {
            this->send_to_esp = true;
        }
    } else {
        this->send_to_esp = false;
    }
}
