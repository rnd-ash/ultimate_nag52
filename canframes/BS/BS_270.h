#ifndef BS_270H
#define BS_270H

#include <stdint.h>
#ifdef FW_MODE
#include <can_common.h>
#endif

#define BS_270_ID 0x0270

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets Pulse ring counter, rear left wheel (48 per revolution)
    void set_RIZ_HL(uint8_t value){ raw = (raw & 0x00ffffffffffffff) | ((uint64_t)value & 0xff) << 56; }
    // Gets Pulse ring counter, rear left wheel (48 per revolution)
    uint8_t get_RIZ_HL() { return raw >> 0 & 0xff; }

    // Sets Pulse ring counter, rear right wheel (48 per revolution)
    void set_RIZ_HR(uint8_t value){ raw = (raw & 0xff00ffffffffffff) | ((uint64_t)value & 0xff) << 48; }
    // Gets Pulse ring counter, rear right wheel (48 per revolution)
    uint8_t get_RIZ_HR() { return raw >> 8 & 0xff; }

    // Sets Flat roll warning status
    void set_PRW_ST(uint8_t value){ raw = (raw & 0xfffff8ffffffffff) | ((uint64_t)value & 0x7) << 40; }
    // Gets Flat roll warning status
    uint8_t get_PRW_ST() { return raw >> 21 & 0x7; }

    // Sets Warning messages flat roll warner
    void set_PRW_WARN(uint8_t value){ raw = (raw & 0xffff0fffffffffff) | ((uint64_t)value & 0xf) << 44; }
    // Gets Warning messages flat roll warner
    uint8_t get_PRW_WARN() { return raw >> 16 & 0xf; }

    void import_frame(CAN_FRAME &f) {
        if (f.id == BS_270_ID) {
            for (int i = 0; i < f.length; i++) {
                bytes[7-i] = f.data.bytes[i];
            }
        }
    }

    void export_frame(CAN_FRAME &f) {
        f.id = BS_270_ID;
        f.length = 8;
        f.priority = 4;
        f.rtr = false;
        f.extended = false;
        for (int i = 0; i < 7; i++) {
            f.data.bytes[i] = bytes[7-i];
        }
    }
} BS_270;

#endif BS_270H
