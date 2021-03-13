#ifndef PSM_3B8_H_
#define PSM_3B8_H_

#include <stdint.h>

#define PSM_3B8_ID 0x03B8

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets speed limit active
    void set_PSM_V_AKT(bool value){ raw = (raw & 0xdfffffffffffffff) | ((uint64_t)value & 0x1) << 61; }
    // Gets speed limit active
    bool get_PSM_V_AKT() { return raw >> 61 & 0x1; }

    // Sets speed limit
    void set_PSM_V_TGL(bool value){ raw = (raw & 0xbfffffffffffffff) | ((uint64_t)value & 0x1) << 62; }
    // Gets speed limit
    bool get_PSM_V_TGL() { return raw >> 62 & 0x1; }

    // Sets speed limit
    void set_PSM_V_PAR(bool value){ raw = (raw & 0x7fffffffffffffff) | ((uint64_t)value & 0x1) << 63; }
    // Gets speed limit
    bool get_PSM_V_PAR() { return raw >> 63 & 0x1; }

    // Sets speed limit
    void set_PSM_V_SOLL(uint8_t value){ raw = (raw & 0xff00ffffffffffff) | ((uint64_t)value & 0xff) << 48; }
    // Gets speed limit
    uint8_t get_PSM_V_SOLL() { return raw >> 48 & 0xff; }

    // Sets Disable accelerator pedal module
    void set_PSM_FPM_SP(bool value){ raw = (raw & 0xfffff7ffffffffff) | ((uint64_t)value & 0x1) << 43; }
    // Gets Disable accelerator pedal module
    bool get_PSM_FPM_SP() { return raw >> 43 & 0x1; }

    // Sets Motor remote stop active
    void set_PSM_FERN_STOP(bool value){ raw = (raw & 0xffffefffffffffff) | ((uint64_t)value & 0x1) << 44; }
    // Gets Motor remote stop active
    bool get_PSM_FERN_STOP() { return raw >> 44 & 0x1; }

    // Sets Motor remote start active
    void set_PSM_FERN_START(bool value){ raw = (raw & 0xffffdfffffffffff) | ((uint64_t)value & 0x1) << 45; }
    // Gets Motor remote start active
    bool get_PSM_FERN_START() { return raw >> 45 & 0x1; }

    // Sets speed limitation
    void set_PSM_DZ_TGL(bool value){ raw = (raw & 0xffffbfffffffffff) | ((uint64_t)value & 0x1) << 46; }
    // Gets speed limitation
    bool get_PSM_DZ_TGL() { return raw >> 46 & 0x1; }

    // Sets speed limitation
    void set_PSM_DZ_PAR(bool value){ raw = (raw & 0xffff7fffffffffff) | ((uint64_t)value & 0x1) << 47; }
    // Gets speed limitation
    bool get_PSM_DZ_PAR() { return raw >> 47 & 0x1; }

    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == PSM_3B8_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = PSM_3B8_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} PSM_3B8;

#endif
