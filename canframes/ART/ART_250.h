#ifndef ART_250H
#define ART_250H

#include <stdint.h>

#ifdef FW_MODE
#include <can_common.h>
#endif

#define ART_250_ID 0x0250

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets Suppression of dynamic full load downshift
    void set_DYN_UNT(bool value){ raw = (raw & 0xfeffffffffffffff) | ((uint64_t)value & 0x1) << 56; }
    // Gets Suppression of dynamic full load downshift
    bool get_DYN_UNT() { return raw >> 7 & 0x1; }

    // Sets brake light suppression
    void set_BL_UNT(bool value){ raw = (raw & 0xfdffffffffffffff) | ((uint64_t)value & 0x1) << 57; }
    // Gets brake light suppression
    bool get_BL_UNT() { return raw >> 6 & 0x1; }

    // Sets ART brakes
    void set_ART_BRE(bool value){ raw = (raw & 0xfbffffffffffffff) | ((uint64_t)value & 0x1) << 58; }
    // Gets ART brakes
    bool get_ART_BRE() { return raw >> 5 & 0x1; }

    // Sets ART ok
    void set_ART_OK(bool value){ raw = (raw & 0xf7ffffffffffffff) | ((uint64_t)value & 0x1) << 59; }
    // Gets ART ok
    bool get_ART_OK() { return raw >> 4 & 0x1; }

    // Sets switching line shift ART
    void set_SLV_ART(uint8_t value){ raw = (raw & 0x0fffffffffffffff) | ((uint64_t)value & 0xf) << 60; }
    // Gets switching line shift ART
    uint8_t get_SLV_ART() { return raw >> 0 & 0xf; }

    // Sets Cityassistent regulates
    void set_CAS_REG(bool value){ raw = (raw & 0xffdfffffffffffff) | ((uint64_t)value & 0x1) << 53; }
    // Gets Cityassistent regulates
    bool get_CAS_REG() { return raw >> 10 & 0x1; }

    // Sets Motor torque request dynamic
    void set_MDYN_ART(bool value){ raw = (raw & 0xffbfffffffffffff) | ((uint64_t)value & 0x1) << 54; }
    // Gets Motor torque request dynamic
    bool get_MDYN_ART() { return raw >> 9 & 0x1; }

    // Sets Motor torque request parity (even parity)
    void set_MPAR_ART(bool value){ raw = (raw & 0xff7fffffffffffff) | ((uint64_t)value & 0x1) << 55; }
    // Gets Motor torque request parity (even parity)
    bool get_MPAR_ART() { return raw >> 8 & 0x1; }

    // Sets ART regulates
    void set_ART_REG(bool value){ raw = (raw & 0xffffdfffffffffff) | ((uint64_t)value & 0x1) << 45; }
    // Gets ART regulates
    bool get_ART_REG() { return raw >> 18 & 0x1; }

    // Sets Limiter controls
    void set_LIM_REG(bool value){ raw = (raw & 0xffffbfffffffffff) | ((uint64_t)value & 0x1) << 46; }
    // Gets Limiter controls
    bool get_LIM_REG() { return raw >> 17 & 0x1; }

    // Sets Required. Engine torque
    void set_M_ART(short value){ raw = (raw & 0xffffe000ffffffff) | ((uint64_t)value & 0x1fff) << 32; }
    // Gets Required. Engine torque
    short get_M_ART() { return raw >> 19 & 0x1fff; }

    // Sets message counter
    void set_BZ250h(uint8_t value){ raw = (raw & 0xffffffff0fffffff) | ((uint64_t)value & 0xf) << 28; }
    // Gets message counter
    uint8_t get_BZ250h() { return raw >> 32 & 0xf; }

    // Sets braking torque (0000h: passive value)
    void set_MBRE_ART(short value){ raw = (raw & 0xfffffffff000ffff) | ((uint64_t)value & 0xfff) << 16; }
    // Gets braking torque (0000h: passive value)
    short get_MBRE_ART() { return raw >> 36 & 0xfff; }

    // Sets target gear, lower limit
    void set_GMIN_ART(uint8_t value){ raw = (raw & 0xfffffffffffff8ff) | ((uint64_t)value & 0x7) << 8; }
    // Gets target gear, lower limit
    uint8_t get_GMIN_ART() { return raw >> 53 & 0x7; }

    // Sets target gear, upper limit
    void set_GMAX_ART(uint8_t value){ raw = (raw & 0xffffffffffffc7ff) | ((uint64_t)value & 0x7) << 11; }
    // Gets target gear, upper limit
    uint8_t get_GMAX_ART() { return raw >> 50 & 0x7; }

    // Sets ART request: "Active downshift"
    void set_AKT_R_ART(bool value){ raw = (raw & 0xffffffffffff7fff) | ((uint64_t)value & 0x1) << 15; }
    // Gets ART request: "Active downshift"
    bool get_AKT_R_ART() { return raw >> 48 & 0x1; }

    void import_frame(CAN_FRAME &f) {
        if (f.id == ART_250_ID) {
            for (int i = 0; i < f.length; i++) {
                bytes[7-i] = f.data.bytes[i];
            }
        }
    }

    void export_frame(CAN_FRAME &f) {
        f.id = ART_250_ID;
        f.length = 8;
        f.priority = 4;
        f.rtr = false;
        f.extended = false;
        for (int i = 0; i < 7; i++) {
            f.data.bytes[i] = bytes[7-i];
        }
    }
} ART_250;

#endif
