#ifndef GS_338_H
#define GS_338_H

#include <stdint.h>
#include <can_common.h>

#define GS_338_ID 0x0338

typedef union {
    uint8_t bytes[8];
    uint64_t raw;
    // Sets gearbox output speed (only 463/461, otherwise FFFFh)
    void set_NAB(short value){ raw = (raw & 0x0000ffffffffffff) | ((uint64_t)value & 0xffff) << 48; }
    // Gets gearbox output speed (only 463/461, otherwise FFFFh)
    short get_NAB() { return raw >> 0 & 0xffff; }

    // Sets turbine speed (EGS52-NAG, VGS-NAG2)
    void set_NTURBINE(short value){ raw = (raw & 0xffffffffffff0000) | ((uint64_t)value & 0xffff) << 0; }
    // Gets turbine speed (EGS52-NAG, VGS-NAG2)
    short get_NTURBINE() { return raw >> 48 & 0xffff; }

    void export_frame(CAN_FRAME &f) {
        f.id = GS_338_ID;
        f.length = 8;
        f.priority = 4;
        f.rtr = false;
        f.extended = false;
        for (int i = 0; i < 7; i++) {
            f.data.bytes[i] = bytes[7-i];
        }
    }
} GS_338;

#endif