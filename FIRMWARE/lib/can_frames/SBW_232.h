#ifndef SBW_232_H_
#define SBW_232_H_

#include <stdint.h>

#define SBW_232_ID 0x0232

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets Steering wheel buttons "+", "-" pressed
    void set_LRT_PM3(uint8_t value){ raw = (raw & 0xf8ffffffffffffff) | ((uint64_t)value & 0x7) << 56; }
    // Gets Steering wheel buttons "+", "-" pressed
    uint8_t get_LRT_PM3() { return raw >> 56 & 0x7; }

    // Sets sender identification
    void set_SID_SBW(uint8_t value){ raw = (raw & 0x3fffffffffffffff) | ((uint64_t)value & 0x3) << 62; }
    // Gets sender identification
    uint8_t get_SID_SBW() { return raw >> 62 & 0x3; }

    // Sets Shift-by-wire control element Status RND
    void set_SBWB_ST_RND(uint8_t value){ raw = (raw & 0xfff0ffffffffffff) | ((uint64_t)value & 0xf) << 48; }
    // Gets Shift-by-wire control element Status RND
    uint8_t get_SBWB_ST_RND() { return raw >> 48 & 0xf; }

    // Sets Shift-by-wire control element P button
    void set_SBWB_ST_P(uint8_t value){ raw = (raw & 0xffcfffffffffffff) | ((uint64_t)value & 0x3) << 52; }
    // Gets Shift-by-wire control element P button
    uint8_t get_SBWB_ST_P() { return raw >> 52 & 0x3; }

    // Sets Shift-by-wire control element identification
    void set_SBWB_ID(uint8_t value){ raw = (raw & 0xff3fffffffffffff) | ((uint64_t)value & 0x3) << 54; }
    // Gets Shift-by-wire control element identification
    uint8_t get_SBWB_ID() { return raw >> 54 & 0x3; }

    // Sets message counter
    void set_BZ232h(uint8_t value){ raw = (raw & 0xffff0fffffffffff) | ((uint64_t)value & 0xf) << 44; }
    // Gets message counter
    uint8_t get_BZ232h() { return raw >> 44 & 0xf; }

    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == SBW_232_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = SBW_232_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} SBW_232;

#endif
