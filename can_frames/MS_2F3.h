#ifndef MS_2F3_H_
#define MS_2F3_H_

#include <stdint.h>
#include "can_c_enums.h"

#define MS_2F3_ID 0x02F3

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets Gear step shift recommendation "Actual"
    void set_FSC_IST(uint8_t value){ raw = (raw & 0x00ffffffffffffff) | ((uint64_t)value & 0xff) << 56; }
    // Gets Gear step shift recommendation "Actual"
    uint8_t get_FSC_IST() { return raw >> 56 & 0xff; }

    // Sets Gear step shift recommendation "target"
    void set_FSC_SOLL(uint8_t value){ raw = (raw & 0xffffffffff00ffff) | ((uint64_t)value & 0xff) << 16; }
    // Gets Gear step shift recommendation "target"
    uint8_t get_FSC_SOLL() { return raw >> 16 & 0xff; }

    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == MS_2F3_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = MS_2F3_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} MS_2F3;

#endif
