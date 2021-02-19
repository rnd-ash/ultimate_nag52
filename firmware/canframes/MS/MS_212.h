#ifndef MS_212H
#define MS_212H

#include <stdint.h>
#include <can_common.h>

#define MS_212_ID 0x0212

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets Target engine idling speed
    void set_NMOTS(short value){ raw = (raw & 0x0000ffffffffffff) | ((uint64_t)value & 0xffff) << 48; }
    // Gets Target engine idling speed
    short get_NMOTS() { return raw >> 0 & 0xffff; }

    // Sets Enable torque request ART
    void set_M_ART_E(bool value){ raw = (raw & 0xffffdfffffffffff) | ((uint64_t)value & 0x1) << 45; }
    // Gets Enable torque request ART
    bool get_M_ART_E() { return raw >> 18 & 0x1; }

    // Sets Series cruise control is variant-coded
    void set_TM_MS(bool value){ raw = (raw & 0xffffbfffffffffff) | ((uint64_t)value & 0x1) << 46; }
    // Gets Series cruise control is variant-coded
    bool get_TM_MS() { return raw >> 17 & 0x1; }

    // Sets driver default torque
    void set_M_FV(short value){ raw = (raw & 0xffffe000ffffffff) | ((uint64_t)value & 0x1fff) << 32; }
    // Gets driver default torque
    short get_M_FV() { return raw >> 19 & 0x1fff; }

    // Sets Enable torque request ESP
    void set_M_ESP_E(bool value){ raw = (raw & 0xffffffffdfffffff) | ((uint64_t)value & 0x1) << 29; }
    // Gets Enable torque request ESP
    bool get_M_ESP_E() { return raw >> 34 & 0x1; }

    // Sets Enable fast torque setting
    void set_SME_E(bool value){ raw = (raw & 0xffffffffbfffffff) | ((uint64_t)value & 0x1) << 30; }
    // Gets Enable fast torque setting
    bool get_SME_E() { return raw >> 33 & 0x1; }

    // Sets Substitute default torque driver
    void set_M_FEV(short value){ raw = (raw & 0xffffffffe000ffff) | ((uint64_t)value & 0x1fff) << 16; }
    // Gets Substitute default torque driver
    short get_M_FEV() { return raw >> 35 & 0x1fff; }

    // Sets Enable torque request EGS
    void set_M_EGS_E(bool value){ raw = (raw & 0xffffffffffffdfff) | ((uint64_t)value & 0x1) << 13; }
    // Gets Enable torque request EGS
    bool get_M_EGS_E() { return raw >> 50 & 0x1; }

    // Sets Acknowledgment of torque request EGS
    void set_M_EGS_Q(bool value){ raw = (raw & 0xffffffffffffbfff) | ((uint64_t)value & 0x1) << 14; }
    // Gets Acknowledgment of torque request EGS
    bool get_M_EGS_Q() { return raw >> 49 & 0x1; }

    // Sets Transfer CALID / CVN enable
    void set_CALID_CVN_E(bool value){ raw = (raw & 0xffffffffffff7fff) | ((uint64_t)value & 0x1) << 15; }
    // Gets Transfer CALID / CVN enable
    bool get_CALID_CVN_E() { return raw >> 48 & 0x1; }

    // Sets Preset torque ESP
    void set_M_ESPV(short value){ raw = (raw & 0xffffffffffffe000) | ((uint64_t)value & 0x1fff) << 0; }
    // Gets Preset torque ESP
    short get_M_ESPV() { return raw >> 51 & 0x1fff; }

    void import_frame(CAN_FRAME &f) {
        if (f.id == MS_212_ID) {
            for (int i = 0; i < f.length; i++) {
                bytes[7-i] = f.data.bytes[i];
            }
        }
    }

    void export_frame(CAN_FRAME &f) {
        f.id = MS_212_ID;
        f.length = 8;
        f.priority = 4;
        f.rtr = false;
        f.extended = false;
        for (int i = 0; i < 7; i++) {
            f.data.bytes[i] = bytes[7-i];
        }
    }
} MS_212;

#endif MS_212H
