#ifndef SBW_232H
#define SBW_232H

#include <stdint.h>

#ifdef FW_MODE
#include <can_common.h>
#endif

#define SBW_232_ID 0x0232

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets Steering wheel buttons "+", "-" pressed
    void set_LRT_PM3(uint8_t value){ raw = (raw & 0xf8ffffffffffffff) | ((uint64_t)value & 0x7) << 56; }
    // Gets Steering wheel buttons "+", "-" pressed
    uint8_t get_LRT_PM3() { return raw >> 5 & 0x7; }

    // Sets sender identification
    void set_SID_SBW(uint8_t value){ raw = (raw & 0x3fffffffffffffff) | ((uint64_t)value & 0x3) << 62; }
    // Gets sender identification
    uint8_t get_SID_SBW() { return raw >> 0 & 0x3; }

    // Sets Shift-by-wire control element Status RND
    void set_SBWB_ST_RND(uint8_t value){ raw = (raw & 0xfff0ffffffffffff) | ((uint64_t)value & 0xf) << 48; }
    // Gets Shift-by-wire control element Status RND
    uint8_t get_SBWB_ST_RND() { return raw >> 12 & 0xf; }

    // Sets Shift-by-wire control element P button
    void set_SBWB_ST_P(uint8_t value){ raw = (raw & 0xffcfffffffffffff) | ((uint64_t)value & 0x3) << 52; }
    // Gets Shift-by-wire control element P button
    uint8_t get_SBWB_ST_P() { return raw >> 10 & 0x3; }

    // Sets Shift-by-wire control element identification
    void set_SBWB_ID(uint8_t value){ raw = (raw & 0xff3fffffffffffff) | ((uint64_t)value & 0x3) << 54; }
    // Gets Shift-by-wire control element identification
    uint8_t get_SBWB_ID() { return raw >> 8 & 0x3; }

    // Sets message counter
    void set_BZ232h(uint8_t value){ raw = (raw & 0xffff0fffffffffff) | ((uint64_t)value & 0xf) << 44; }
    // Gets message counter
    uint8_t get_BZ232h() { return raw >> 16 & 0xf; }

    void import_frame(CAN_FRAME &f) {
        if (f.id == SBW_232_ID) {
            for (int i = 0; i < f.length; i++) {
                bytes[7-i] = f.data.bytes[i];
            }
        }
    }

    void export_frame(CAN_FRAME &f) {
        f.id = SBW_232_ID;
        f.length = 8;
        f.priority = 4;
        f.rtr = false;
        f.extended = false;
        for (int i = 0; i < 7; i++) {
            f.data.bytes[i] = bytes[7-i];
        }
    }
} SBW_232;

#endif SBW_232H
