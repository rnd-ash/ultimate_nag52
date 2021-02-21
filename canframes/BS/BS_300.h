#ifndef BS_300H
#define BS_300H

#include <stdint.h>

#ifdef FW_MODE
#include <can_common.h>
#endif

#define BS_300_ID 0x0300

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets Steering angle sensor initialization possible
    void set_LWS_INI_EIN(bool value){ raw = (raw & 0xfeffffffffffffff) | ((uint64_t)value & 0x1) << 56; }
    // Gets Steering angle sensor initialization possible
    bool get_LWS_INI_EIN() { return raw >> 56 & 0x1; }

    // Sets Initialization of steering angle sensor o.k.
    void set_LWS_INI_OK(bool value){ raw = (raw & 0xfdffffffffffffff) | ((uint64_t)value & 0x1) << 57; }
    // Gets Initialization of steering angle sensor o.k.
    bool get_LWS_INI_OK() { return raw >> 57 & 0x1; }

    // Sets ESP yaw moment control active
    void set_ESP_GIER_AKT(bool value){ raw = (raw & 0xfbffffffffffffff) | ((uint64_t)value & 0x1) << 58; }
    // Gets ESP yaw moment control active
    bool get_ESP_GIER_AKT() { return raw >> 58 & 0x1; }

    // Sets Enable ART
    void set_ART_E(bool value){ raw = (raw & 0xf7ffffffffffffff) | ((uint64_t)value & 0x1) << 59; }
    // Gets Enable ART
    bool get_ART_E() { return raw >> 59 & 0x1; }

    // Sets Full braking (ABS controls all 4 wheels)
    void set_VOLLBRE(bool value){ raw = (raw & 0xefffffffffffffff) | ((uint64_t)value & 0x1) << 60; }
    // Gets Full braking (ABS controls all 4 wheels)
    bool get_VOLLBRE() { return raw >> 60 & 0x1; }

    // Sets BAS control active
    void set_BAS_AKT(bool value){ raw = (raw & 0xdfffffffffffffff) | ((uint64_t)value & 0x1) << 61; }
    // Gets BAS control active
    bool get_BAS_AKT() { return raw >> 61 & 0x1; }

    // Sets Dynamic engine torque request
    void set_DMDYN_ART(bool value){ raw = (raw & 0xbfffffffffffffff) | ((uint64_t)value & 0x1) << 62; }
    // Gets Dynamic engine torque request
    bool get_DMDYN_ART() { return raw >> 62 & 0x1; }

    // Sets Motor torque request parity (even parity)
    void set_DMPAR_ART(bool value){ raw = (raw & 0x7fffffffffffffff) | ((uint64_t)value & 0x1) << 63; }
    // Gets Motor torque request parity (even parity)
    bool get_DMPAR_ART() { return raw >> 63 & 0x1; }

    // Sets driver brakes
    void set_SFB(uint8_t value){ raw = (raw & 0xfffcffffffffffff) | ((uint64_t)value & 0x3) << 48; }
    // Gets driver brakes
    uint8_t get_SFB() { return raw >> 48 & 0x3; }

    // Sets driver brakes parity (even parity)
    void set_SFB_PA(bool value){ raw = (raw & 0xfffbffffffffffff) | ((uint64_t)value & 0x1) << 50; }
    // Gets driver brakes parity (even parity)
    bool get_SFB_PA() { return raw >> 50 & 0x1; }

    // Sets transmission cycle time
    void set_T_Z(uint8_t value){ raw = (raw & 0xffe7ffffffffffff) | ((uint64_t)value & 0x3) << 51; }
    // Gets transmission cycle time
    uint8_t get_T_Z() { return raw >> 51 & 0x3; }

    // Sets drive torque control active
    void set_AMR_AKT_ESP(bool value){ raw = (raw & 0xffdfffffffffffff) | ((uint64_t)value & 0x1) << 53; }
    // Gets drive torque control active
    bool get_AMR_AKT_ESP() { return raw >> 53 & 0x1; }

    // Sets Dynamic motor torque request
    void set_MDYN_ESP(bool value){ raw = (raw & 0xffbfffffffffffff) | ((uint64_t)value & 0x1) << 54; }
    // Gets Dynamic motor torque request
    bool get_MDYN_ESP() { return raw >> 54 & 0x1; }

    // Sets Motor torque request parity (even parity)
    void set_MPAR_ESP(bool value){ raw = (raw & 0xff7fffffffffffff) | ((uint64_t)value & 0x1) << 55; }
    // Gets Motor torque request parity (even parity)
    bool get_MPAR_ESP() { return raw >> 55 & 0x1; }

    // Sets Motor torque request Max
    void set_DMMAX_ART(bool value){ raw = (raw & 0xffffdfffffffffff) | ((uint64_t)value & 0x1) << 45; }
    // Gets Motor torque request Max
    bool get_DMMAX_ART() { return raw >> 45 & 0x1; }

    // Sets engine torque request min
    void set_DMMIN_ART(bool value){ raw = (raw & 0xffffbfffffffffff) | ((uint64_t)value & 0x1) << 46; }
    // Gets engine torque request min
    bool get_DMMIN_ART() { return raw >> 46 & 0x1; }

    // Sets engine torque request. Toggle 40ms + -10
    void set_DMTGL_ART(bool value){ raw = (raw & 0xffff7fffffffffff) | ((uint64_t)value & 0x1) << 47; }
    // Gets engine torque request. Toggle 40ms + -10
    bool get_DMTGL_ART() { return raw >> 47 & 0x1; }

    // Sets Required. Engine torque
    void set_DM_ART(short value){ raw = (raw & 0xffffe000ffffffff) | ((uint64_t)value & 0x1fff) << 32; }
    // Gets Required. Engine torque
    short get_DM_ART() { return raw >> 32 & 0x1fff; }

    // Sets Motor torque request Max
    void set_MMAX_ESP(bool value){ raw = (raw & 0xffffffffdfffffff) | ((uint64_t)value & 0x1) << 29; }
    // Gets Motor torque request Max
    bool get_MMAX_ESP() { return raw >> 29 & 0x1; }

    // Sets Motor torque request Min
    void set_MMIN_ESP(bool value){ raw = (raw & 0xffffffffbfffffff) | ((uint64_t)value & 0x1) << 30; }
    // Gets Motor torque request Min
    bool get_MMIN_ESP() { return raw >> 30 & 0x1; }

    // Sets motor torque request. Toggle 40ms + -10
    void set_MTGL_ESP(bool value){ raw = (raw & 0xffffffff7fffffff) | ((uint64_t)value & 0x1) << 31; }
    // Gets motor torque request. Toggle 40ms + -10
    bool get_MTGL_ESP() { return raw >> 31 & 0x1; }

    // Sets Required. Engine torque
    void set_M_ESP(short value){ raw = (raw & 0xffffffffe000ffff) | ((uint64_t)value & 0x1fff) << 16; }
    // Gets Required. Engine torque
    short get_M_ESP() { return raw >> 16 & 0x1fff; }

    // Sets Raw signal yaw rate without adjustment / filtering (+ = left)
    void set_GIER_ROH(short value){ raw = (raw & 0xffffffffffff0000) | ((uint64_t)value & 0xffff) << 0; }
    // Gets Raw signal yaw rate without adjustment / filtering (+ = left)
    short get_GIER_ROH() { return raw >> 0 & 0xffff; }

    void import_frame(CAN_FRAME &f) {
        if (f.id == BS_300_ID) {
            for (int i = 0; i < f.length; i++) {
                bytes[7-i] = f.data.bytes[i];
            }
        }
    }

    void export_frame(CAN_FRAME &f) {
        f.id = BS_300_ID;
        f.length = 8;
        f.priority = 4;
        f.rtr = false;
        f.extended = false;
        for (int i = 0; i < 7; i++) {
            f.data.bytes[i] = bytes[7-i];
        }
    }
} BS_300;

#endif
