#ifndef MS_312H
#define MS_312H

#include <stdint.h>
#ifdef FW_MODE
#include <can_common.h>
#endif

#define MS_312_ID 0x0312

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets Static motor torque
    void set_M_STA(short value){ raw = (raw & 0xe000ffffffffffff) | ((uint64_t)value & 0x1fff) << 48; }
    // Gets Static motor torque
    short get_M_STA() { return raw >> 3 & 0x1fff; }

    // Sets Maximum motor torque including dyn. Turbocharger share
    void set_M_MAX_ATL(short value){ raw = (raw & 0xffffe000ffffffff) | ((uint64_t)value & 0x1fff) << 32; }
    // Gets Maximum motor torque including dyn. Turbocharger share
    short get_M_MAX_ATL() { return raw >> 19 & 0x1fff; }

    // Sets maximum motor torque
    void set_M_MAX(short value){ raw = (raw & 0xffffffffe000ffff) | ((uint64_t)value & 0x1fff) << 16; }
    // Gets maximum motor torque
    short get_M_MAX() { return raw >> 35 & 0x1fff; }

    // Sets minimum motor torque
    void set_M_MIN(short value){ raw = (raw & 0xffffffffffffe000) | ((uint64_t)value & 0x1fff) << 0; }
    // Gets minimum motor torque
    short get_M_MIN() { return raw >> 51 & 0x1fff; }

    void import_frame(CAN_FRAME &f) {
        if (f.id == MS_312_ID) {
            for (int i = 0; i < f.length; i++) {
                bytes[7-i] = f.data.bytes[i];
            }
        }
    }

    void export_frame(CAN_FRAME &f) {
        f.id = MS_312_ID;
        f.length = 8;
        f.priority = 4;
        f.rtr = false;
        f.extended = false;
        for (int i = 0; i < 7; i++) {
            f.data.bytes[i] = bytes[7-i];
        }
    }
} MS_312;

#endif
