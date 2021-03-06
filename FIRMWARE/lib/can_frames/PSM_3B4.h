#ifndef PSM_3B4_H_
#define PSM_3B4_H_

#include <stdint.h>

#define PSM_3B4_ID 0x03B4

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets Working speed control active
    void set_PSM_ADR_AKT(bool value){ raw = (raw & 0xdfffffffffffffff) | ((uint64_t)value & 0x1) << 61; }
    // Gets Working speed control active
    bool get_PSM_ADR_AKT() { return raw >> 61 & 0x1; }

    // Sets working speed control
    void set_PSM_ADR_TGL(bool value){ raw = (raw & 0xbfffffffffffffff) | ((uint64_t)value & 0x1) << 62; }
    // Gets working speed control
    bool get_PSM_ADR_TGL() { return raw >> 62 & 0x1; }

    // Sets working speed control
    void set_PSM_ADR_PAR(bool value){ raw = (raw & 0x7fffffffffffffff) | ((uint64_t)value & 0x1) << 63; }
    // Gets working speed control
    bool get_PSM_ADR_PAR() { return raw >> 63 & 0x1; }

    // Sets Motor target speed ADR
    void set_PSM_N_SOLL(short value){ raw = (raw & 0xff0000ffffffffff) | ((uint64_t)value & 0xffff) << 40; }
    // Gets Motor target speed ADR
    short get_PSM_N_SOLL() { return raw >> 40 & 0xffff; }

    // Sets torque limitation active
    void set_PSM_MOM_AKT(bool value){ raw = (raw & 0xffffffdfffffffff) | ((uint64_t)value & 0x1) << 37; }
    // Gets torque limitation active
    bool get_PSM_MOM_AKT() { return raw >> 37 & 0x1; }

    // Sets torque limit
    void set_PSM_MOM_TGL(bool value){ raw = (raw & 0xffffffbfffffffff) | ((uint64_t)value & 0x1) << 38; }
    // Gets torque limit
    bool get_PSM_MOM_TGL() { return raw >> 38 & 0x1; }

    // Sets torque limitation
    void set_PSM_MOM_PAR(bool value){ raw = (raw & 0xffffff7fffffffff) | ((uint64_t)value & 0x1) << 39; }
    // Gets torque limitation
    bool get_PSM_MOM_PAR() { return raw >> 39 & 0x1; }

    // Sets Maximum motor torque
    void set_PSM_MOM_SOLL(short value){ raw = (raw & 0xffffffe000ffffff) | ((uint64_t)value & 0x1fff) << 24; }
    // Gets Maximum motor torque
    short get_PSM_MOM_SOLL() { return raw >> 24 & 0x1fff; }

    // Sets speed limitation active
    void set_PSM_DZ_AKT(bool value){ raw = (raw & 0xffffffffffdfffff) | ((uint64_t)value & 0x1) << 21; }
    // Gets speed limitation active
    bool get_PSM_DZ_AKT() { return raw >> 21 & 0x1; }

    // Sets speed limitation
    void set_PSM_DZ_TGL(bool value){ raw = (raw & 0xffffffffffbfffff) | ((uint64_t)value & 0x1) << 22; }
    // Gets speed limitation
    bool get_PSM_DZ_TGL() { return raw >> 22 & 0x1; }

    // Sets speed limitation
    void set_PSM_DZ_PAR(bool value){ raw = (raw & 0xffffffffff7fffff) | ((uint64_t)value & 0x1) << 23; }
    // Gets speed limitation
    bool get_PSM_DZ_PAR() { return raw >> 23 & 0x1; }

    // Sets maximum speed
    void set_PSM_DZ_MAX(short value){ raw = (raw & 0xffffffffffff0000) | ((uint64_t)value & 0xffff) << 0; }
    // Gets maximum speed
    short get_PSM_DZ_MAX() { return raw >> 0 & 0xffff; }

    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == PSM_3B4_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = PSM_3B4_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} PSM_3B4;

#endif
