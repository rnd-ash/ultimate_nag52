//
// Created by ashcon on 2/20/21.
//

#include "car_sim.h"
#include "esp32_forwarder.h"

#include <utility>
#include <cstring>
#include <fstream>

#include "EZS_A5.h"
EZS_A5 ezsa5; // For variant testing!

CAR_SIMULATOR *sim = nullptr;

std::string fmt_frame(CAN_FRAME *f) {
    char buf[150] = {0x00};
    int pos = sprintf(buf, "0x%04X - [", f->id);
    for (int i = 0; i < f->dlc; i++) {
        pos += sprintf(buf+pos, "%02X ", f->data[i]);
    }
    buf[pos-1] = ']';
    return std::string(buf);
}

void CAR_SIMULATOR::init() {
    this->esp_ecu = new class abs_esp();
    this->nag52_ecu = new class nag52();
    this->eng_ecu = new class engine();
    this->ewm_ecu = new class ewm();

    this->ecus.push_back((abstract_ecu*)this->esp_ecu);
    this->ecus.push_back((abstract_ecu*)this->eng_ecu);
    this->ecus.push_back((abstract_ecu*)this->ewm_ecu);

    this->ecus.push_back((abstract_ecu*)this->nag52_ecu);
    for (auto x : this->ecus){
        x->setup();
    }

    printf("Init with %lu ecus!\n", ecus.size());
    this->thread_exec = true;
    this->can_thread = std::thread(&CAR_SIMULATOR::can_sim_thread, this);
    this->sim_thread = std::thread(&CAR_SIMULATOR::ecu_sim_thread, this);
    sim = this;
}

// Replay mode!
void CAR_SIMULATOR::init(char *file_name) {
    this->esp_ecu = new class abs_esp();
    this->eng_ecu = new class engine();
    this->ewm_ecu = new class ewm();
    this->nag52_ecu = new class nag52();

    this->ecus.push_back((abstract_ecu*)this->esp_ecu);
    this->ecus.push_back((abstract_ecu*)this->eng_ecu);
    this->ecus.push_back((abstract_ecu*)this->ewm_ecu);
    this->ecus.push_back((abstract_ecu*)this->nag52_ecu);

    std::vector<CAN_FRAME> f;
    std::string line;
    std::ifstream input(file_name);
    for (std::string line; getline(input, line);) {
        int sep_pos = line.find('#');
        if (sep_pos != std::string::npos) {
            CAN_FRAME frame = {0x00};
            int can_id = stoi(line.substr(0, sep_pos));
            char* data = (char*)&line.c_str()[sep_pos+1];
            int pos = 0;
            int length = strlen(data);
            char buf[4];
            int buf_pos = 0;
            memset(buf, 0x00, 4);
            while (pos <= length) {
                if (data[pos] == ',' || pos == length) {
                    uint8_t b = atoi(buf);
                    frame.data[frame.dlc] = b;
                    frame.dlc += 1;
                    buf_pos = 0;
                    memset(buf, 0x00, 4);
                } else {
                    buf[buf_pos] = data[pos];
                    buf_pos += 1;
                }
                pos += 1;
            }
            frame.id = can_id;
            f.push_back(frame);
        }
    }
    printf("Replay mode active!\n");
    this->sim_thread = std::thread(&CAR_SIMULATOR::ecu_sim_thread_replay, this, f);
    sim = this;
}

void CAR_SIMULATOR::ecu_sim_thread_replay(std::vector<CAN_FRAME> f) {
    CAN_FRAME tx;
    ezsa5.raw = 0x020024062D18181A;
    ezsa5.set_NAG_VH(true);
    ezsa5.set_KSG_VH(false);

#define SPEED_MULTIPLIER 5

    int ticks = 0;
    printf("Replay thread start!\n");
    for (auto i: f) {
        //this->bcast_frame(&i);
        if (ticks == 50 * SPEED_MULTIPLIER) {
            ticks = 0;
            // Send EZS frame to keep IC thinking engine is running
            TX_FRAME(ezsa5)
            TX_FRAME(ms210)
            TX_FRAME(ms212)
            TX_FRAME(ms268)
            //TX_FRAME(ms2F3)
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
            TX_FRAME(art258)
            TX_FRAME(ewm230)
            TX_FRAME(lrw236)
            TX_FRAME(mrm238)
            TX_FRAME(sbw232)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        bool bcast = true;
        switch(i.id) {
            case MS_210_ID:
                ms210.import_frame(MS_210_ID, i.data, i.dlc);
                break;
            case MS_212_ID:
                ms212.import_frame(MS_212_ID, i.data, i.dlc);
                break;
            case MS_268_ID:
                ms268.import_frame(MS_268_ID, i.data, i.dlc);
                break;
            case MS_2F3_ID:
                ms2f3.import_frame(MS_2F3_ID, i.data, i.dlc);
                break;
            case MS_308_ID:
                ms308.import_frame(MS_308_ID, i.data, i.dlc);
                break;
            case MS_312_ID:
                ms312.import_frame(MS_312_ID, i.data, i.dlc);
                break;
            case MS_608_ID:
                ms608.import_frame(MS_608_ID, i.data, i.dlc);
                break;
            case BS_200_ID:
                bs200.import_frame(BS_200_ID, i.data, i.dlc);
                break;
            case BS_208_ID:
                bs200.import_frame(BS_208_ID, i.data, i.dlc);
                break;
            case BS_270_ID:
                bs270.import_frame(BS_270_ID, i.data, i.dlc);
                break;
            case BS_300_ID:
                bs300.import_frame(BS_300_ID, i.data, i.dlc);
                break;
            case BS_328_ID:
                bs328.import_frame(BS_328_ID, i.data, i.dlc);
                break;
            case ART_250_ID:
                art250.import_frame(ART_250_ID, i.data, i.dlc);
                break;
            case ART_258_ID:
                art258.import_frame(ART_258_ID, i.data, i.dlc);
                break;
            case GS_418_ID:
                gs418.import_frame(GS_418_ID, i.data, i.dlc);
                break;
            case GS_338_ID:
                gs338.import_frame(GS_338_ID, i.data, i.dlc);
                break;
            case GS_218_ID:
                gs218.import_frame(GS_218_ID, i.data, i.dlc);
                break;
            case LRW_236_ID:
                lrw236.import_frame(LRW_236_ID, i.data, i.dlc);
                break;
            case MRM_238_ID:
                mrm238.import_frame(MRM_238_ID, i.data, i.dlc);
                break;
            case SBW_232_ID:
                sbw232.import_frame(SBW_232_ID, i.data, i.dlc);
                break;
            default:
                bcast = false;
                break;
        }
        ticks++;
    }
}

uint8_t distance = 50;
void CAR_SIMULATOR::can_sim_thread() {
    printf("CAN sender thread starting!\n");
    CAN_FRAME tx;
    ezsa5.raw = 0x020024062D18181A;
    art258.raw = 0x0000000000000000;

    ezsa5.set_NAG_VH(true);
    ezsa5.set_KSG_VH(false);
    //art258.set_ART_ERR(0);
    //art258.set_ART_INFO(true);
    ////art258.set_ART_WT(true);
    //art258.set_S_OBJ(true);
    //art258.set_ART_SEG_EIN(true);
    ////art258.set_ART_EIN(true);
    //art258.set_ASSIST_FKT_AKT(3);
    //art258.set_ASSIST_ANZ_V2(13);
    //art258.set_V_ART(50);
    //art258.set_ART_DSPL_EIN(true);
    //art258.set_OBJ_ERK(true);
    //art258.set_ABST_R_OBJ(distance); // In yards
    //art258.set_V_ZIEL(10);
    //art258.set_ART_ABW_AKT(true);
    //art258.set_AAS_LED_BL(true);
    //art258.set_OBJ_AGB(true);
    //art258.set_CAS_ERR_ANZ_V2(1);

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
        //TX_FRAME(ms2F3)
        TX_FRAME(ms308)
        //TX_FRAME(ms312)
        TX_FRAME(ms608)
        TX_FRAME(gs218)
        TX_FRAME(gs338)
        TX_FRAME(gs418)
        TX_FRAME(gs419)
        TX_FRAME(bs200)
        TX_FRAME(bs208)
        TX_FRAME(bs270)
        TX_FRAME(bs300)
        TX_FRAME(bs328)
        TX_FRAME(art258)
        TX_FRAME(ewm230)
        TX_FRAME(lrw236)
        TX_FRAME(mrm238)
        TX_FRAME(sbw232)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void CAR_SIMULATOR::ecu_sim_thread() {
    printf("ECU Simulator thread starting!\n");
    while(this->thread_exec) {
        for (auto ecu: this->ecus) {
            ecu->simulate_tick();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void CAR_SIMULATOR::terminate() {
    this->thread_exec = false;
    this->can_thread.join();
    this->sim_thread.join();
    if (this->send_to_esp) {
        this->esp32.close_port();
    }
    CLEAR_FRAME(ms210)
    CLEAR_FRAME(ms212)
    CLEAR_FRAME(ms268)
    //CLEAR_FRAME(ms2F3)
    CLEAR_FRAME(ms308)
    CLEAR_FRAME(ms312)
    CLEAR_FRAME(ms608)
    CLEAR_FRAME(gs218)
    CLEAR_FRAME(gs338)
    CLEAR_FRAME(gs418)
    CLEAR_FRAME(bs200)
    CLEAR_FRAME(bs208)
    CLEAR_FRAME(bs270)
    CLEAR_FRAME(bs300)
    CLEAR_FRAME(bs328)
}

void CAR_SIMULATOR::bcast_frame(CAN_FRAME *f) {
    this->last_frames[f->id] = *f; // Copy to last frames
    if (this->send_to_esp) {
        this->esp32.send_frame(f);
    }
}

CAR_SIMULATOR::CAR_SIMULATOR(char *port_name) {
    if (port_name != nullptr) {
        this->esp32 = esp32_forwarder(port_name);
        if (this->esp32.is_port_open()) {
            this->send_to_esp = true;
        }
    } else {
        this->send_to_esp = false;
    }
}

abs_esp* CAR_SIMULATOR::get_abs() {
    return this->esp_ecu;
}

ewm* CAR_SIMULATOR::get_ewm() {
    return this->ewm_ecu;
}

engine* CAR_SIMULATOR::get_engine() {
    return this->eng_ecu;
}

nag52* CAR_SIMULATOR::get_nag52() {
    return this->nag52_ecu;
}
