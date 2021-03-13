#ifndef BS_328_H_
#define BS_328_H_

#include <stdint.h>
#include "can_c_enums.h"

#define BS_328_ID 0x0328

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets WMS Parity (even parity)
    void set_WMS_PA(bool value){ raw = (raw & 0x7fffffffffffffff) | ((uint64_t)value & 0x1) << 63; }
    // Gets WMS Parity (even parity)
    bool get_WMS_PA() { return raw >> 63 & 0x1; }

    // Sets Set roll torque change
    void set_WMS(short value){ raw = (raw & 0x8000ffffffffffff) | ((uint64_t)value & 0x7fff) << 48; }
    // Gets Set roll torque change
    short get_WMS() { return raw >> 48 & 0x7fff; }

    // Sets vehicle lateral acceleration. in the center of gravity (+ = left)
    void set_AY_S(uint8_t value){ raw = (raw & 0xffff00ffffffffff) | ((uint64_t)value & 0xff) << 40; }
    // Gets vehicle lateral acceleration. in the center of gravity (+ = left)
    uint8_t get_AY_S() { return raw >> 40 & 0xff; }

    // Sets ESP display messages
    void set_ESP_DSPL(uint8_t value){ raw = (raw & 0xffffffffe0ffffff) | ((uint64_t)value & 0x1f) << 24; }
    // Gets ESP display messages
    uint8_t get_ESP_DSPL() { return raw >> 24 & 0x1f; }

    // Sets message counter
    void set_BZ328h(uint8_t value){ raw = (raw & 0xfffffffffff8ffff) | ((uint64_t)value & 0x7) << 16; }
    // Gets message counter
    uint8_t get_BZ328h() { return raw >> 16 & 0x7; }

    // Sets open clutch
    void set_KPL_OEF(bool value){ raw = (raw & 0xfffffffffff7ffff) | ((uint64_t)value & 0x1) << 19; }
    // Gets open clutch
    bool get_KPL_OEF() { return raw >> 19 & 0x1; }

    // Sets emergency braking (brake light flashing)
    void set_NOTBRE(bool value){ raw = (raw & 0xffffffffffbfffff) | ((uint64_t)value & 0x1) << 22; }
    // Gets emergency braking (brake light flashing)
    bool get_NOTBRE() { return raw >> 22 & 0x1; }

    // Sets Pulse ring counter, front left wheel (48 per revolution)
    void set_RIZ_VL(uint8_t value){ raw = (raw & 0xffffffffffff00ff) | ((uint64_t)value & 0xff) << 8; }
    // Gets Pulse ring counter, front left wheel (48 per revolution)
    uint8_t get_RIZ_VL() { return raw >> 8 & 0xff; }

    // Sets Pulse ring counter, front right wheel (48 per revolution)
    void set_RIZ_VR(uint8_t value){ raw = (raw & 0xffffffffffffff00) | ((uint64_t)value & 0xff) << 0; }
    // Gets Pulse ring counter, front right wheel (48 per revolution)
    uint8_t get_RIZ_VR() { return raw >> 0 & 0xff; }

    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == BS_328_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = BS_328_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} BS_328;

#endif
