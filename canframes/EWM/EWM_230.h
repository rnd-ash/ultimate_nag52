#ifndef EWM_230_H_
#define EWM_230_H_

#include <stdint.h>

#ifdef FW_MODE
#include <can_common.h>
#endif

#define EWM_230_ID 0x0230

enum WHC {
    GS_D = 5,
    GS_N = 6,
    GS_R = 7,
    GS_P = 8,
    GS_PLUS = 9,
    GS_MINUS = 10,
    GS_N_D = 11,
    GS_R_N = 12,
    GS_P_R = 13,
    GS_SNV = 15
};


typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets gear selector lever position (only NAG)
    void set_WHC(WHC value){ raw = (raw & 0xf0ffffffffffffff) | ((uint64_t)value & 0xf) << 56; }
    // Gets gear selector lever position (only NAG)
    WHC get_WHC() { return (WHC)(raw >> 56 & 0xf); }

    // Sets Locking magnet energized
    void set_LOCKING(bool value){ raw = (raw & 0xefffffffffffffff) | ((uint64_t)value & 0x1) << 60; }
    // Gets Locking magnet energized
    bool get_LOCKING() { return raw >> 60 & 0x1; }

    // Sets kickdown
    void set_KD(bool value){ raw = (raw & 0xdfffffffffffffff) | ((uint64_t)value & 0x1) << 61; }
    // Gets kickdown
    bool get_KD() { return raw >> 61 & 0x1; }

    // Sets Drive program button pressed
    void set_FPT(bool value){ raw = (raw & 0xbfffffffffffffff) | ((uint64_t)value & 0x1) << 62; }
    // Gets Drive program button pressed
    bool get_FPT() { return raw >> 62 & 0x1; }

    // Sets drive program
    void set_W_S(bool value){ raw = (raw & 0x7fffffffffffffff) | ((uint64_t)value & 0x1) << 63; }
    // Gets drive program
    bool get_W_S() { return raw >> 63 & 0x1; }

    void import_frame(CAN_FRAME &f) {
        if (f.id == EWM_230_ID) {
            for (int i = 0; i < f.length; i++) {
                bytes[7-i] = f.data.bytes[i];
            }
        }
    }

    void export_frame(CAN_FRAME &f) {
        f.id = EWM_230_ID;
        f.length = 8;
        f.priority = 4;
        f.rtr = false;
        f.extended = false;
        for (int i = 0; i < 7; i++) {
            f.data.bytes[i] = bytes[7-i];
        }
    }
} EWM_230;

#endif
