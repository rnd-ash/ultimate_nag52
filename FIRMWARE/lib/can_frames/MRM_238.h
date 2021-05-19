#ifndef MRM_238_H_
#define MRM_238_H_

#include <stdint.h>
#include <can_c_enums.h>

#define MRM_238_ID 0x0238

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets Cruise control lever: "Switch off"
    void set_OFF(bool value){ raw = (raw & 0xfeffffffffffffff) | ((uint64_t)value & 0x1) << 56; }
    // Gets Cruise control lever: "Switch off"
    bool get_OFF() { return raw >> 56 & 0x1; }

    // Sets Cruise control lever: "Resume"
    void set_WA(bool value){ raw = (raw & 0xfdffffffffffffff) | ((uint64_t)value & 0x1) << 57; }
    // Gets Cruise control lever: "Resume"
    bool get_WA() { return raw >> 57 & 0x1; }

    // Sets Cruise control lever: "Set and accelerate level0"
    void set_S_PLUS_B(bool value){ raw = (raw & 0xfbffffffffffffff) | ((uint64_t)value & 0x1) << 58; }
    // Gets Cruise control lever: "Set and accelerate level0"
    bool get_S_PLUS_B() { return raw >> 58 & 0x1; }

    // Sets Cruise control lever: "Set and decelerate level0"
    void set_S_MINUS_B(bool value){ raw = (raw & 0xf7ffffffffffffff) | ((uint64_t)value & 0x1) << 59; }
    // Gets Cruise control lever: "Set and decelerate level0"
    bool get_S_MINUS_B() { return raw >> 59 & 0x1; }

    // Sets Operation variable speed limit
    void set_VMAX_AKT(bool value){ raw = (raw & 0xefffffffffffffff) | ((uint64_t)value & 0x1) << 60; }
    // Gets Operation variable speed limit
    bool get_VMAX_AKT() { return raw >> 60 & 0x1; }

    // Sets Cruise control lever implausible
    void set_WH_UP(bool value){ raw = (raw & 0xdfffffffffffffff) | ((uint64_t)value & 0x1) << 61; }
    // Gets Cruise control lever implausible
    bool get_WH_UP() { return raw >> 61 & 0x1; }

    // Sets message counter
    void set_BZ238h(uint8_t value){ raw = (raw & 0xfff0ffffffffffff) | ((uint64_t)value & 0xf) << 48; }
    // Gets message counter
    uint8_t get_BZ238h() { return raw >> 48 & 0xf; }

    // Sets Cruise control lever parity (even parity)
    void set_WH_PA(bool value){ raw = (raw & 0xffefffffffffffff) | ((uint64_t)value & 0x1) << 52; }
    // Gets Cruise control lever parity (even parity)
    bool get_WH_PA() { return raw >> 52 & 0x1; }

    // Sets direction flashing left
    void set_BLI_LI(bool value){ raw = (raw & 0xffbfffffffffffff) | ((uint64_t)value & 0x1) << 54; }
    // Gets direction flashing left
    bool get_BLI_LI() { return raw >> 54 & 0x1; }

    // Sets right direction flashing
    void set_BLI_RE(bool value){ raw = (raw & 0xff7fffffffffffff) | ((uint64_t)value & 0x1) << 55; }
    // Gets right direction flashing
    bool get_BLI_RE() { return raw >> 55 & 0x1; }

    // Sets steering angle sign
    void set_LW_VZ(bool value){ raw = (raw & 0xfffff7ffffffffff) | ((uint64_t)value & 0x1) << 43; }
    // Gets steering angle sign
    bool get_LW_VZ() { return raw >> 43 & 0x1; }

    // Sets Steering angle sensor: not initialized
    void set_LW_INI(bool value){ raw = (raw & 0xffffefffffffffff) | ((uint64_t)value & 0x1) << 44; }
    // Gets Steering angle sensor: not initialized
    bool get_LW_INI() { return raw >> 44 & 0x1; }

    // Sets Steering angle sensor: Code error
    void set_LW_CF(bool value){ raw = (raw & 0xffffdfffffffffff) | ((uint64_t)value & 0x1) << 45; }
    // Gets Steering angle sensor: Code error
    bool get_LW_CF() { return raw >> 45 & 0x1; }

    // Sets Steering angle sensor: overflow
    void set_LW_OV(bool value){ raw = (raw & 0xffffbfffffffffff) | ((uint64_t)value & 0x1) << 46; }
    // Gets Steering angle sensor: overflow
    bool get_LW_OV() { return raw >> 46 & 0x1; }

    // Sets Steering angle parity (even parity)
    void set_LW_PA(bool value){ raw = (raw & 0xffff7fffffffffff) | ((uint64_t)value & 0x1) << 47; }
    // Gets Steering angle parity (even parity)
    bool get_LW_PA() { return raw >> 47 & 0x1; }

    // Sets steering angle
    void set_LW(short value){ raw = (raw & 0xfffff800ffffffff) | ((uint64_t)value & 0x7ff) << 32; }
    // Gets steering angle
    short get_LW() { return raw >> 32 & 0x7ff; }

    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == MRM_238_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = MRM_238_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} MRM_238;

#endif
