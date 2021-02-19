#ifndef MS_2F3H
#define MS_2F3H

#include <stdint.h>
#include <can_common.h>

#define MS_2F3_ID 0x02F3

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets Gear step shift recommendation "Actual"
    void set_FSC_IST(uint8_t value){ raw = (raw & 0x00ffffffffffffff) | ((uint64_t)value & 0xff) << 56; }
    // Gets Gear step shift recommendation "Actual"
    uint8_t get_FSC_IST() { return raw >> 0 & 0xff; }

    // Sets Gear step shift recommendation "target"
    void set_FSC_SOLL(uint8_t value){ raw = (raw & 0xffffffffff00ffff) | ((uint64_t)value & 0xff) << 16; }
    // Gets Gear step shift recommendation "target"
    uint8_t get_FSC_SOLL() { return raw >> 40 & 0xff; }

    void import_frame(CAN_FRAME &f) {
        if (f.id == MS_2F3_ID) {
            for (int i = 0; i < f.length; i++) {
                bytes[7-i] = f.data.bytes[i];
            }
        }
    }

    void export_frame(CAN_FRAME &f) {
        f.id = MS_2F3_ID;
        f.length = 8;
        f.priority = 4;
        f.rtr = false;
        f.extended = false;
        for (int i = 0; i < 7; i++) {
            f.data.bytes[i] = bytes[7-i];
        }
    }
} MS_2F3;

#endif MS_2F3H
