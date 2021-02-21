#ifndef GS_418H
#define GS_418H

#include <stdint.h>
#include "enums.h"

#ifdef FW_MODE
#include <can_common.h>
#endif

#define GS_418_ID 0x0418

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets speed step
    void set_FSC(uint8_t value){ raw = (raw & 0x00ffffffffffffff) | ((uint64_t)value & 0xff) << 56; }
    // Gets speed step
    uint8_t get_FSC() { return raw >> 56 & 0xff; }

    // Sets driving program
    void set_FPC(uint8_t value){ raw = (raw & 0xff00ffffffffffff) | ((uint64_t)value & 0xff) << 48; }
    // Gets driving program
    uint8_t get_FPC() { return raw >> 48 & 0xff; }

    // Sets transmission oil temperature
    void set_T_GET(uint8_t value){ raw = (raw & 0xffff00ffffffffff) | ((uint64_t)value & 0xff) << 40; }
    // Gets transmission oil temperature
    uint8_t get_T_GET() { return raw >> 40 & 0xff; }

    // Sets Kickdown
    void set_KD(bool value){ raw = (raw & 0xfffffffeffffffff) | ((uint64_t)value & 0x1) << 32; }
    // Gets Kickdown
    bool get_KD() { return raw >> 32 & 0x1; }

    // Sets apply brake when switching on
    void set_ESV_BRE(bool value){ raw = (raw & 0xfffffffdffffffff) | ((uint64_t)value & 0x1) << 33; }
    // Gets apply brake when switching on
    bool get_ESV_BRE() { return raw >> 33 & 0x1; }

    // Sets gear mechanism variant
    void set_MECH(uint8_t value){ raw = (raw & 0xfffffff3ffffffff) | ((uint64_t)value & 0x3) << 34; }
    // Gets gear mechanism variant
    uint8_t get_MECH() { return raw >> 34 & 0x3; }

    // Sets continuously variable transmission [1], multi-step transmission [0]
    void set_CVT(bool value){ raw = (raw & 0xffffffefffffffff) | ((uint64_t)value & 0x1) << 36; }
    // Gets continuously variable transmission [1], multi-step transmission [0]
    bool get_CVT() { return raw >> 36 & 0x1; }

    // Sets switching
    void set_SWITCH(bool value){ raw = (raw & 0xffffffdfffffffff) | ((uint64_t)value & 0x1) << 37; }
    // Gets switching
    bool get_SWITCH() { return raw >> 37 & 0x1; }

    // Sets front-wheel drive [1], rear-wheel drive [0]
    void set_FRONT(bool value){ raw = (raw & 0xffffffbfffffffff) | ((uint64_t)value & 0x1) << 38; }
    // Gets front-wheel drive [1], rear-wheel drive [0]
    bool get_FRONT() { return raw >> 38 & 0x1; }

    // Sets all-wheel drive
    void set_ALL_WHEEL(bool value){ raw = (raw & 0xffffff7fffffffff) | ((uint64_t)value & 0x1) << 39; }
    // Gets all-wheel drive
    bool get_ALL_WHEEL() { return raw >> 39 & 0x1; }

    // Sets actual gear
    void set_GIC(uint8_t value){ raw = (raw & 0xfffffffff0ffffff) | ((uint64_t)value & 0xf) << 24; }
    // Gets actual gear
    uint8_t get_GIC() { return raw >> 24 & 0xf; }

    // Sets target gear
    void set_GZC(uint8_t value){ raw = (raw & 0xffffffff0fffffff) | ((uint64_t)value & 0xf) << 28; }
    // Gets target gear
    uint8_t get_GZC() { return raw >> 28 & 0xf; }

    // Sets torque loss (FFh at KSG)
    void set_M_VERL(uint8_t value){ raw = (raw & 0xffffffffff00ffff) | ((uint64_t)value & 0xff) << 16; }
    // Gets torque loss (FFh at KSG)
    uint8_t get_M_VERL() { return raw >> 16 & 0xff; }

    // Sets Gear selector lever position (NAG, KSG, CVT)
    void set_WHST(uint8_t value){ raw = (raw & 0xffffffffffffc7ff) | ((uint64_t)value & 0x7) << 11; }
    // Gets Gear selector lever position (NAG, KSG, CVT)
    uint8_t get_WHST() { return raw >> 11 & 0x7; }

    // Sets Factor wheel torque toggle 40ms + -10
    void set_FMRADTGL(bool value){ raw = (raw & 0xffffffffffffbfff) | ((uint64_t)value & 0x1) << 14; }
    // Gets Factor wheel torque toggle 40ms + -10
    bool get_FMRADTGL() { return raw >> 14 & 0x1; }

    // Sets Wheel torque parity factor (even parity)
    void set_FMRADPAR(bool value){ raw = (raw & 0xffffffffffff7fff) | ((uint64_t)value & 0x1) << 15; }
    // Gets Wheel torque parity factor (even parity)
    bool get_FMRADPAR() { return raw >> 15 & 0x1; }

    // Sets wheel torque factor (7FFh at KSG)
    void set_FMRAD(short value){ raw = (raw & 0xfffffffffffff800) | ((uint64_t)value & 0x7ff) << 0; }
    // Gets wheel torque factor (7FFh at KSG)
    short get_FMRAD() { return raw >> 0 & 0x7ff; }

    void import_frame(CAN_FRAME &f) {
        if (f.id == GS_418_ID) {
            for (int i = 0; i < f.length; i++) {
                bytes[7-i] = f.data.bytes[i];
            }
        }
    }

    void export_frame(CAN_FRAME &f) {
        f.id = GS_418_ID;
        f.length = 8;
        f.priority = 4;
        f.rtr = false;
        f.extended = false;
        for (int i = 0; i < 7; i++) {
            f.data.bytes[i] = bytes[7-i];
        }
    }
} GS_418;

#endif
