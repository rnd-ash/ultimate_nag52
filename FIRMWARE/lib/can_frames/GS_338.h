#ifndef GS_338_H_
#define GS_338_H_

#include <stdint.h>
#include "can_c_enums.h"

#define GS_338_ID 0x0338

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets gearbox output speed (only 463/461, otherwise FFFFh)
    void set_NAB(short value){ raw = (raw & 0x0000ffffffffffff) | ((uint64_t)value & 0xffff) << 48; }
    // Gets gearbox output speed (only 463/461, otherwise FFFFh)
    short get_NAB() { return raw >> 48 & 0xffff; }

    // Sets turbine speed (EGS52-NAG, VGS-NAG2)
    void set_NTURBINE(short value){ raw = (raw & 0xffffffffffff0000) | ((uint64_t)value & 0xffff) << 0; }
    // Gets turbine speed (EGS52-NAG, VGS-NAG2)
    short get_NTURBINE() { return raw >> 0 & 0xffff; }

    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == GS_338_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = GS_338_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} GS_338;

#endif
