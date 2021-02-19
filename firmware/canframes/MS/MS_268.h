#ifndef MS_268H
#define MS_268H

#include <stdint.h>
#include <can_common.h>

#define MS_268_ID 0x0268

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets target ratio, lower limit (FCVT)
    void set_IMIN_MS(uint8_t value){ raw = (raw & 0x00ffffffffffffff) | ((uint64_t)value & 0xff) << 56; }
    // Gets target ratio, lower limit (FCVT)
    uint8_t get_IMIN_MS() { return raw >> 0 & 0xff; }

    // Sets target ratio, upper limit (FCVT)
    void set_IMAX_MS(uint8_t value){ raw = (raw & 0xff00ffffffffffff) | ((uint64_t)value & 0xff) << 48; }
    // Gets target ratio, upper limit (FCVT)
    uint8_t get_IMAX_MS() { return raw >> 8 & 0xff; }

    // Sets Status of heating output
    void set_HZL_ST(uint8_t value){ raw = (raw & 0xfffffcffffffffff) | ((uint64_t)value & 0x3) << 40; }
    // Gets Status of heating output
    uint8_t get_HZL_ST() { return raw >> 22 & 0x3; }

    // Sets shut-off valve cooling circuit M266 ATL
    void set_ASV_KKL_169(bool value){ raw = (raw & 0xffffdfffffffffff) | ((uint64_t)value & 0x1) << 45; }
    // Gets shut-off valve cooling circuit M266 ATL
    bool get_ASV_KKL_169() { return raw >> 18 & 0x1; }

    // Sets Oil info, reserved M266
    void set_OEL_INFO_169(bool value){ raw = (raw & 0xffffbfffffffffff) | ((uint64_t)value & 0x1) << 46; }
    // Gets Oil info, reserved M266
    bool get_OEL_INFO_169() { return raw >> 17 & 0x1; }

    // Sets Terminal 61
    void set_KL_61_EIN(bool value){ raw = (raw & 0xffff7fffffffffff) | ((uint64_t)value & 0x1) << 47; }
    // Gets Terminal 61
    bool get_KL_61_EIN() { return raw >> 16 & 0x1; }

    // Sets Generator utilization (only with LIN generators!)
    void set_LAST_GEN(uint8_t value){ raw = (raw & 0xffffffc0ffffffff) | ((uint64_t)value & 0x3f) << 32; }
    // Gets Generator utilization (only with LIN generators!)
    uint8_t get_LAST_GEN() { return raw >> 26 & 0x3f; }

    // Sets Air control system mode
    void set_LRS_MODE(bool value){ raw = (raw & 0xffffffbfffffffff) | ((uint64_t)value & 0x1) << 38; }
    // Gets Air control system mode
    bool get_LRS_MODE() { return raw >> 25 & 0x1; }

    // Sets Request force-free in "D" (FCVT)
    void set_KID_MS(bool value){ raw = (raw & 0xffffff7fffffffff) | ((uint64_t)value & 0x1) << 39; }
    // Gets Request force-free in "D" (FCVT)
    bool get_KID_MS() { return raw >> 24 & 0x1; }

    // Sets Max. Air conditioning compressor torque
    void set_M_KOMP_MAX(uint8_t value){ raw = (raw & 0xffffffff00ffffff) | ((uint64_t)value & 0xff) << 24; }
    // Gets Max. Air conditioning compressor torque
    uint8_t get_M_KOMP_MAX() { return raw >> 32 & 0xff; }

    // Sets pedal value driver (only 169)
    void set_PW_F(uint8_t value){ raw = (raw & 0xffffffffff00ffff) | ((uint64_t)value & 0xff) << 16; }
    // Gets pedal value driver (only 169)
    uint8_t get_PW_F() { return raw >> 40 & 0xff; }

    void import_frame(CAN_FRAME &f) {
        if (f.id == MS_268_ID) {
            for (int i = 0; i < f.length; i++) {
                bytes[7-i] = f.data.bytes[i];
            }
        }
    }

    void export_frame(CAN_FRAME &f) {
        f.id = MS_268_ID;
        f.length = 8;
        f.priority = 4;
        f.rtr = false;
        f.extended = false;
        for (int i = 0; i < 7; i++) {
            f.data.bytes[i] = bytes[7-i];
        }
    }
} MS_268;

#endif MS_268H
