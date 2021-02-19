#ifndef MS_608H
#define MS_608H

#include <stdint.h>
#include <can_common.h>

#define MS_608_ID 0x0608

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets engine coolant temperature
    void set_T_MOT(uint8_t value){ raw = (raw & 0x00ffffffffffffff) | ((uint64_t)value & 0xff) << 56; }
    // Gets engine coolant temperature
    uint8_t get_T_MOT() { return raw >> 0 & 0xff; }

    // Sets intake air temperature
    void set_T_LUFT(uint8_t value){ raw = (raw & 0xff00ffffffffffff) | ((uint64_t)value & 0xff) << 48; }
    // Gets intake air temperature
    uint8_t get_T_LUFT() { return raw >> 8 & 0xff; }

    // Sets vehicle code series
    void set_FCOD_BR(uint8_t value){ raw = (raw & 0xffffe0ffffffffff) | ((uint64_t)value & 0x1f) << 40; }
    // Gets vehicle code series
    uint8_t get_FCOD_BR() { return raw >> 19 & 0x1f; }

    // Sets vehicle code body
    void set_FCOD_KAR(uint8_t value){ raw = (raw & 0xffff1fffffffffff) | ((uint64_t)value & 0x7) << 45; }
    // Gets vehicle code body
    uint8_t get_FCOD_KAR() { return raw >> 16 & 0x7; }

    // Sets Vehicle code motor 7Bit, Bit0-5 (Bit6 -> Signal FCOD_MOT6)
    void set_FCOD_MOT(uint8_t value){ raw = (raw & 0xffffffc0ffffffff) | ((uint64_t)value & 0x3f) << 32; }
    // Gets Vehicle code motor 7Bit, Bit0-5 (Bit6 -> Signal FCOD_MOT6)
    uint8_t get_FCOD_MOT() { return raw >> 26 & 0x3f; }

    // Sets transmission control not available
    void set_GS_NVH(bool value){ raw = (raw & 0xffffffbfffffffff) | ((uint64_t)value & 0x1) << 38; }
    // Gets transmission control not available
    bool get_GS_NVH() { return raw >> 25 & 0x1; }

    // Sets Vehicle code engine with 7 bits, bit 6
    void set_FCOD_MOT6(bool value){ raw = (raw & 0xffffff7fffffffff) | ((uint64_t)value & 0x1) << 39; }
    // Gets Vehicle code engine with 7 bits, bit 6
    bool get_FCOD_MOT6() { return raw >> 24 & 0x1; }

    // Sets Fixed maximum speed
    void set_V_MAX_FIX(uint8_t value){ raw = (raw & 0xffffffff00ffffff) | ((uint64_t)value & 0xff) << 24; }
    // Gets Fixed maximum speed
    uint8_t get_V_MAX_FIX() { return raw >> 32 & 0xff; }

    // Sets consumption
    void set_VB(short value){ raw = (raw & 0xffffffffff0000ff) | ((uint64_t)value & 0xffff) << 8; }
    // Gets consumption
    short get_VB() { return raw >> 40 & 0xffff; }

    // Sets Particle filter correction offset FMMOTMAX
    void set_PFKO(uint8_t value){ raw = (raw & 0xfffffffffffffff0) | ((uint64_t)value & 0xf) << 0; }
    // Gets Particle filter correction offset FMMOTMAX
    uint8_t get_PFKO() { return raw >> 60 & 0xf; }

    // Sets switch on additional consumer
    void set_ZVB_EIN_MS(bool value){ raw = (raw & 0xffffffffffffffef) | ((uint64_t)value & 0x1) << 4; }
    // Gets switch on additional consumer
    bool get_ZVB_EIN_MS() { return raw >> 59 & 0x1; }

    // Sets Particle filter warning
    void set_PFW(uint8_t value){ raw = (raw & 0xffffffffffffff9f) | ((uint64_t)value & 0x3) << 5; }
    // Gets Particle filter warning
    uint8_t get_PFW() { return raw >> 57 & 0x3; }

    // Sets switch on additional water pump
    void set_ZWP_EIN_MS(bool value){ raw = (raw & 0xffffffffffffff7f) | ((uint64_t)value & 0x1) << 7; }
    // Gets switch on additional water pump
    bool get_ZWP_EIN_MS() { return raw >> 56 & 0x1; }

    void import_frame(CAN_FRAME &f) {
        if (f.id == MS_608_ID) {
            for (int i = 0; i < f.length; i++) {
                bytes[7-i] = f.data.bytes[i];
            }
        }
    }

    void export_frame(CAN_FRAME &f) {
        f.id = MS_608_ID;
        f.length = 8;
        f.priority = 4;
        f.rtr = false;
        f.extended = false;
        for (int i = 0; i < 7; i++) {
            f.data.bytes[i] = bytes[7-i];
        }
    }
} MS_608;

#endif MS_608H
