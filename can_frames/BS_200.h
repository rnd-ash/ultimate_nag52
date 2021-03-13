#ifndef BS_200_H_
#define BS_200_H_

#include <stdint.h>
#include "can_c_enums.h"

#define BS_200_ID 0x0200

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets Brake pad wear indicator lamp
    void set_BBV_KL(bool value){ raw = (raw & 0xfeffffffffffffff) | ((uint64_t)value & 0x1) << 56; }
    // Gets Brake pad wear indicator lamp
    bool get_BBV_KL() { return raw >> 56 & 0x1; }

    // Sets ABS defective control lamp
    void set_ABS_KL(bool value){ raw = (raw & 0xfbffffffffffffff) | ((uint64_t)value & 0x1) << 58; }
    // Gets ABS defective control lamp
    bool get_ABS_KL() { return raw >> 58 & 0x1; }

    // Sets ESP defective control lamp
    void set_ESP_KL(bool value){ raw = (raw & 0xf7ffffffffffffff) | ((uint64_t)value & 0x1) << 59; }
    // Gets ESP defective control lamp
    bool get_ESP_KL() { return raw >> 59 & 0x1; }

    // Sets ESP info lamp steady light
    void set_ESP_INFO_DL(bool value){ raw = (raw & 0xefffffffffffffff) | ((uint64_t)value & 0x1) << 60; }
    // Gets ESP info lamp steady light
    bool get_ESP_INFO_DL() { return raw >> 60 & 0x1; }

    // Sets ESP info lamp flashing light
    void set_ESP_INFO_BL(bool value){ raw = (raw & 0xdfffffffffffffff) | ((uint64_t)value & 0x1) << 61; }
    // Gets ESP info lamp flashing light
    bool get_ESP_INFO_BL() { return raw >> 61 & 0x1; }

    // Sets BAS defective control lamp
    void set_BAS_KL(bool value){ raw = (raw & 0xbfffffffffffffff) | ((uint64_t)value & 0x1) << 62; }
    // Gets BAS defective control lamp
    bool get_BAS_KL() { return raw >> 62 & 0x1; }

    // Sets brake defective control lamp (EBV_KL at 463/461 / NCV2)
    void set_BRE_KL(bool value){ raw = (raw & 0x7fffffffffffffff) | ((uint64_t)value & 0x1) << 63; }
    // Gets brake defective control lamp (EBV_KL at 463/461 / NCV2)
    bool get_BRE_KL() { return raw >> 63 & 0x1; }

    // Sets brake light switch
    void set_BLS(uint8_t value){ raw = (raw & 0xfffcffffffffffff) | ((uint64_t)value & 0x3) << 48; }
    // Gets brake light switch
    uint8_t get_BLS() { return raw >> 48 & 0x3; }

    // Sets message counter
    void set_BZ200h(uint8_t value){ raw = (raw & 0xffc3ffffffffffff) | ((uint64_t)value & 0xf) << 50; }
    // Gets message counter
    uint8_t get_BZ200h() { return raw >> 50 & 0xf; }

    // Sets BLS Parity (even parity)
    void set_BLS_PA(bool value){ raw = (raw & 0xffbfffffffffffff) | ((uint64_t)value & 0x1) << 54; }
    // Gets BLS Parity (even parity)
    bool get_BLS_PA() { return raw >> 54 & 0x1; }

    // Sets brake light suppression (EBV_KL at 163 / T0 / T1N)
    void set_BLS_UNT(bool value){ raw = (raw & 0xff7fffffffffffff) | ((uint64_t)value & 0x1) << 55; }
    // Gets brake light suppression (EBV_KL at 163 / T0 / T1N)
    bool get_BLS_UNT() { return raw >> 55 & 0x1; }

    // Sets direction of rotation of front left wheel
    void set_DRTGVL(uint8_t value){ raw = (raw & 0xffff3fffffffffff) | ((uint64_t)value & 0x3) << 46; }
    // Gets direction of rotation of front left wheel
    uint8_t get_DRTGVL() { return raw >> 46 & 0x3; }

    // Sets wheel speed front left
    void set_DVL(short value){ raw = (raw & 0xffffc000ffffffff) | ((uint64_t)value & 0x3fff) << 32; }
    // Gets wheel speed front left
    short get_DVL() { return raw >> 32 & 0x3fff; }

    // Sets direction of rotation of the front right wheel
    void set_DRTGVR(uint8_t value){ raw = (raw & 0xffffffff3fffffff) | ((uint64_t)value & 0x3) << 30; }
    // Gets direction of rotation of the front right wheel
    uint8_t get_DRTGVR() { return raw >> 30 & 0x3; }

    // Sets wheel speed front right
    void set_DVR(short value){ raw = (raw & 0xffffffffc000ffff) | ((uint64_t)value & 0x3fff) << 16; }
    // Gets wheel speed front right
    short get_DVR() { return raw >> 16 & 0x3fff; }

    // Sets direction of rotation wheel left for cruise control
    void set_DRTGTM(uint8_t value){ raw = (raw & 0xffffffffffff3fff) | ((uint64_t)value & 0x3) << 14; }
    // Gets direction of rotation wheel left for cruise control
    uint8_t get_DRTGTM() { return raw >> 14 & 0x3; }

    // Sets Left wheel speed for cruise control
    void set_TM_DL(short value){ raw = (raw & 0xffffffffffffc000) | ((uint64_t)value & 0x3fff) << 0; }
    // Gets Left wheel speed for cruise control
    short get_TM_DL() { return raw >> 0 & 0x3fff; }

    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == BS_200_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = BS_200_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} BS_200;

#endif
