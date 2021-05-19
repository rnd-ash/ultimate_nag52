#ifndef EWM_230_H_
#define EWM_230_H_

#include <stdint.h>
#include "can_c_enums.h"

#define EWM_230_ID 0x0230

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets gear selector lever position (only NAG)
    void set_WHC(EWM_WHC value){ raw = (raw & 0xf0ffffffffffffff) | ((uint64_t)value & 0xf) << 56; }
    // Gets gear selector lever position (only NAG)
    EWM_WHC get_WHC() { return (EWM_WHC)(raw >> 56 & 0xf); }

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

    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == EWM_230_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = EWM_230_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} EWM_230;

#endif
