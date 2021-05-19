#ifndef MS_312_H_
#define MS_312_H_

#include <stdint.h>
#include "can_c_enums.h"

#define MS_312_ID 0x0312

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets Static motor torque
    void set_M_STA(short value){ raw = (raw & 0xe000ffffffffffff) | ((uint64_t)value & 0x1fff) << 48; }
    // Gets Static motor torque
    short get_M_STA() { return raw >> 48 & 0x1fff; }

    // Sets Maximum motor torque including dyn. Turbocharger share
    void set_M_MAX_ATL(short value){ raw = (raw & 0xffffe000ffffffff) | ((uint64_t)value & 0x1fff) << 32; }
    // Gets Maximum motor torque including dyn. Turbocharger share
    short get_M_MAX_ATL() { return raw >> 32 & 0x1fff; }

    // Sets maximum motor torque
    void set_M_MAX(short value){ raw = (raw & 0xffffffffe000ffff) | ((uint64_t)value & 0x1fff) << 16; }
    // Gets maximum motor torque
    short get_M_MAX() { return raw >> 16 & 0x1fff; }

    // Sets minimum motor torque
    void set_M_MIN(short value){ raw = (raw & 0xffffffffffffe000) | ((uint64_t)value & 0x1fff) << 0; }
    // Gets minimum motor torque
    short get_M_MIN() { return raw >> 0 & 0x1fff; }

    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == MS_312_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = MS_312_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} MS_312;

#endif
