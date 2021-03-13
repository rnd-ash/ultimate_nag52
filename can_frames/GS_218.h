#ifndef GS_218_H_
#define GS_218_H_

#include <stdint.h>
#include "can_c_enums.h"

#define GS_218_ID 0x0218

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets Motor torque request Max
    void set_MMAX_EGS(bool value){ raw = (raw & 0xdfffffffffffffff) | ((uint64_t)value & 0x1) << 61; }
    // Gets Motor torque request Max
    bool get_MMAX_EGS() { return raw >> 61 & 0x1; }

    // Sets engine torque request min
    void set_MMIN_EGS(bool value){ raw = (raw & 0xbfffffffffffffff) | ((uint64_t)value & 0x1) << 62; }
    // Gets engine torque request min
    bool get_MMIN_EGS() { return raw >> 62 & 0x1; }

    // Sets motor torque request. Toggle 40ms + -10
    void set_MTGL_EGS(bool value){ raw = (raw & 0x7fffffffffffffff) | ((uint64_t)value & 0x1) << 63; }
    // Gets motor torque request. Toggle 40ms + -10
    bool get_MTGL_EGS() { return raw >> 63 & 0x1; }

    // Sets Required. Engine torque
    void set_M_EGS(short value){ raw = (raw & 0xe000ffffffffffff) | ((uint64_t)value & 0x1fff) << 48; }
    // Gets Required. Engine torque
    short get_M_EGS() { return raw >> 48 & 0x1fff; }

    // Sets actual gear
    void set_GIC(GS_GIC value){ raw = (raw & 0xfffff0ffffffffff) | ((uint64_t)value & 0xf) << 40; }
    // Gets actual gear
    GS_GIC get_GIC() { return (GS_GIC)(raw >> 40 & 0xf); }

    // Sets target gear
    void set_GZC(GS_GZC value){ raw = (raw & 0xffff0fffffffffff) | ((uint64_t)value & 0xf) << 44; }
    // Gets target gear
    GS_GZC get_GZC() { return (GS_GZC)(raw >> 44 & 0xf); }

    // Sets manual switching mode
    void set_HSM(bool value){ raw = (raw & 0xfffffffeffffffff) | ((uint64_t)value & 0x1) << 32; }
    // Gets manual switching mode
    bool get_HSM() { return raw >> 32 & 0x1; }

    // Sets switching
    void set_SWITCH(bool value){ raw = (raw & 0xfffffffdffffffff) | ((uint64_t)value & 0x1) << 33; }
    // Gets switching
    bool get_SWITCH() { return raw >> 33 & 0x1; }

    // Sets driving resistance high
    void set_FW_HOCH(bool value){ raw = (raw & 0xfffffffbffffffff) | ((uint64_t)value & 0x1) << 34; }
    // Gets driving resistance high
    bool get_FW_HOCH() { return raw >> 34 & 0x1; }

    // Sets basic switching program o.k.
    void set_GSP_OK(bool value){ raw = (raw & 0xfffffff7ffffffff) | ((uint64_t)value & 0x1) << 35; }
    // Gets basic switching program o.k.
    bool get_GSP_OK() { return raw >> 35 & 0x1; }

    // Sets off-road gear
    void set_G_G(bool value){ raw = (raw & 0xffffffefffffffff) | ((uint64_t)value & 0x1) << 36; }
    // Gets off-road gear
    bool get_G_G() { return raw >> 36 & 0x1; }

    // Sets Order (converter bypass) clutch "closed"
    void set_K_G_B(bool value){ raw = (raw & 0xffffffdfffffffff) | ((uint64_t)value & 0x1) << 37; }
    // Gets Order (converter bypass) clutch "closed"
    bool get_K_G_B() { return raw >> 37 & 0x1; }

    // Sets Order (converter bypass) clutch "open"
    void set_K_O_B(bool value){ raw = (raw & 0xffffffbfffffffff) | ((uint64_t)value & 0x1) << 38; }
    // Gets Order (converter bypass) clutch "open"
    bool get_K_O_B() { return raw >> 38 & 0x1; }

    // Sets Order (converter bypass) clutch "slip"
    void set_K_S_B(bool value){ raw = (raw & 0xffffff7fffffffff) | ((uint64_t)value & 0x1) << 39; }
    // Gets Order (converter bypass) clutch "slip"
    bool get_K_S_B() { return raw >> 39 & 0x1; }

    // Sets drive program for AAD
    void set_FPC_AAD(GS_FPC_AAD value){ raw = (raw & 0xfffffffffcffffff) | ((uint64_t)value & 0x3) << 24; }
    // Gets drive program for AAD
    GS_FPC_AAD get_FPC_AAD() { return (GS_FPC_AAD)(raw >> 24 & 0x3); }

    // Sets Kickdown
    void set_KD(bool value){ raw = (raw & 0xfffffffffbffffff) | ((uint64_t)value & 0x1) << 26; }
    // Gets Kickdown
    bool get_KD() { return raw >> 26 & 0x1; }

    // Sets Overtemperature gearbox
    void set_UEHITZ_GET(bool value){ raw = (raw & 0xfffffffff7ffffff) | ((uint64_t)value & 0x1) << 27; }
    // Gets Overtemperature gearbox
    bool get_UEHITZ_GET() { return raw >> 27 & 0x1; }

    // Sets GS in emergency mode
    void set_GS_NOTL(bool value){ raw = (raw & 0xffffffffefffffff) | ((uint64_t)value & 0x1) << 28; }
    // Gets GS in emergency mode
    bool get_GS_NOTL() { return raw >> 28 & 0x1; }

    // Sets start release
    void set_ALF(bool value){ raw = (raw & 0xffffffffdfffffff) | ((uint64_t)value & 0x1) << 29; }
    // Gets start release
    bool get_ALF() { return raw >> 29 & 0x1; }

    // Sets bang start
    void set_KS(bool value){ raw = (raw & 0xffffffffbfffffff) | ((uint64_t)value & 0x1) << 30; }
    // Gets bang start
    bool get_KS() { return raw >> 30 & 0x1; }

    // Sets gear ok
    void set_GET_OK(bool value){ raw = (raw & 0xffffffff7fffffff) | ((uint64_t)value & 0x1) << 31; }
    // Gets gear ok
    bool get_GET_OK() { return raw >> 31 & 0x1; }

    // Sets Motor emergency shutdown
    void set_MOT_NAUS(bool value){ raw = (raw & 0xfffffffffffeffff) | ((uint64_t)value & 0x1) << 16; }
    // Gets Motor emergency shutdown
    bool get_MOT_NAUS() { return raw >> 16 & 0x1; }

    // Sets MOT_NAUS confirmbit
    void set_MOT_NAUS_CNF(bool value){ raw = (raw & 0xfffffffffffdffff) | ((uint64_t)value & 0x1) << 17; }
    // Gets MOT_NAUS confirmbit
    bool get_MOT_NAUS_CNF() { return raw >> 17 & 0x1; }

    // Sets converter lockup clutch unloaded
    void set_K_LSTFR(bool value){ raw = (raw & 0xfffffffffffbffff) | ((uint64_t)value & 0x1) << 18; }
    // Gets converter lockup clutch unloaded
    bool get_K_LSTFR() { return raw >> 18 & 0x1; }

    // Sets intervention mode / drive torque control
    void set_DYN0_AMR_EGS(bool value){ raw = (raw & 0xffffffffffdfffff) | ((uint64_t)value & 0x1) << 21; }
    // Gets intervention mode / drive torque control
    bool get_DYN0_AMR_EGS() { return raw >> 21 & 0x1; }

    // Sets intervention mode / drive torque control
    void set_DYN1_EGS(bool value){ raw = (raw & 0xffffffffffbfffff) | ((uint64_t)value & 0x1) << 22; }
    // Gets intervention mode / drive torque control
    bool get_DYN1_EGS() { return raw >> 22 & 0x1; }

    // Sets Motor torque request parity (even parity)
    void set_MPAR_EGS(bool value){ raw = (raw & 0xffffffffff7fffff) | ((uint64_t)value & 0x1) << 23; }
    // Gets Motor torque request parity (even parity)
    bool get_MPAR_EGS() { return raw >> 23 & 0x1; }

    // Sets creep torque (FFh with EGS, CVT) or CALID / CVN
    void set_MKRIECH(uint8_t value){ raw = (raw & 0xffffffffffff00ff) | ((uint64_t)value & 0xff) << 8; }
    // Gets creep torque (FFh with EGS, CVT) or CALID / CVN
    uint8_t get_MKRIECH() { return raw >> 8 & 0xff; }

    // Sets Error number or counter for CALID / CVN transmission
    void set_ERROR(uint8_t value){ raw = (raw & 0xffffffffffffffe0) | ((uint64_t)value & 0x1f) << 0; }
    // Gets Error number or counter for CALID / CVN transmission
    uint8_t get_ERROR() { return raw >> 0 & 0x1f; }

    // Sets CALID / CVN transmission active
    void set_CALID_CVN_AKT(bool value){ raw = (raw & 0xffffffffffffffdf) | ((uint64_t)value & 0x1) << 5; }
    // Gets CALID / CVN transmission active
    bool get_CALID_CVN_AKT() { return raw >> 5 & 0x1; }

    // Sets Status error check
    void set_FEHLPRF_ST(GS_ST value){ raw = (raw & 0xffffffffffffff3f) | ((uint64_t)value & 0x3) << 6; }
    // Gets Status error check
    GS_ST get_FEHLPRF_ST() { return (GS_ST)(raw >> 6 & 0x3); }

    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == GS_218_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = GS_218_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} GS_218;

#endif
