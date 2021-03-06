#ifndef BS_208_H_
#define BS_208_H_

#include <stdint.h>
#include "can_c_enums.h"

#define BS_208_ID 0x0208

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets target gear, lower limit
    void set_GMIN_ESP(uint8_t value){ raw = (raw & 0xf8ffffffffffffff) | ((uint64_t)value & 0x7) << 56; }
    // Gets target gear, lower limit
    uint8_t get_GMIN_ESP() { return raw >> 56 & 0x7; }

    // Sets target gear, upper limit
    void set_GMAX_ESP(uint8_t value){ raw = (raw & 0xc7ffffffffffffff) | ((uint64_t)value & 0x7) << 59; }
    // Gets target gear, upper limit
    uint8_t get_GMAX_ESP() { return raw >> 59 & 0x7; }

    // Sets Target gear request from ART
    void set_MINMAX_ART(bool value){ raw = (raw & 0xbfffffffffffffff) | ((uint64_t)value & 0x1) << 62; }
    // Gets Target gear request from ART
    bool get_MINMAX_ART() { return raw >> 62 & 0x1; }

    // Sets ESP / ART request: "Active downshift"
    void set_AKT_R_ESP(bool value){ raw = (raw & 0x7fffffffffffffff) | ((uint64_t)value & 0x1) << 63; }
    // Gets ESP / ART request: "Active downshift"
    bool get_AKT_R_ESP() { return raw >> 63 & 0x1; }

    // Sets switching line shift ESP
    void set_SLV_ESP(uint8_t value){ raw = (raw & 0xfff0ffffffffffff) | ((uint64_t)value & 0xf) << 48; }
    // Gets switching line shift ESP
    uint8_t get_SLV_ESP() { return raw >> 48 & 0xf; }

    // Sets Cruise control mode off
    void set_TM_AUS(bool value){ raw = (raw & 0xffefffffffffffff) | ((uint64_t)value & 0x1) << 52; }
    // Gets Cruise control mode off
    bool get_TM_AUS() { return raw >> 52 & 0x1; }

    // Sets system status
    void set_SZS(uint8_t value){ raw = (raw & 0xff9fffffffffffff) | ((uint64_t)value & 0x3) << 53; }
    // Gets system status
    uint8_t get_SZS() { return raw >> 53 & 0x3; }

    // Sets Suppression of dynamic full load downshift
    void set_DDYN_UNT(bool value){ raw = (raw & 0xff7fffffffffffff) | ((uint64_t)value & 0x1) << 55; }
    // Gets Suppression of dynamic full load downshift
    bool get_DDYN_UNT() { return raw >> 55 & 0x1; }

    // Sets ART brake intervention active
    void set_BRE_AKT_ART(bool value){ raw = (raw & 0xffffefffffffffff) | ((uint64_t)value & 0x1) << 44; }
    // Gets ART brake intervention active
    bool get_BRE_AKT_ART() { return raw >> 44 & 0x1; }

    // Sets ESP request: insert "N"
    void set_ANFN(uint8_t value){ raw = (raw & 0xffff9fffffffffff) | ((uint64_t)value & 0x3) << 45; }
    // Gets ESP request: insert "N"
    uint8_t get_ANFN() { return raw >> 45 & 0x3; }

    // Sets ESP brake intervention active
    void set_BRE_AKT_ESP(bool value){ raw = (raw & 0xffff7fffffffffff) | ((uint64_t)value & 0x1) << 47; }
    // Gets ESP brake intervention active
    bool get_BRE_AKT_ESP() { return raw >> 47 & 0x1; }

    // Sets Set braking torque (BR240 factor 1.8 greater)
    void set_MBRE_ESP(short value){ raw = (raw & 0xfffff000ffffffff) | ((uint64_t)value & 0xfff) << 32; }
    // Gets Set braking torque (BR240 factor 1.8 greater)
    short get_MBRE_ESP() { return raw >> 32 & 0xfff; }

    // Sets direction of rotation of rear wheel right
    void set_DRTGHR(uint8_t value){ raw = (raw & 0xffffffff3fffffff) | ((uint64_t)value & 0x3) << 30; }
    // Gets direction of rotation of rear wheel right
    uint8_t get_DRTGHR() { return raw >> 30 & 0x3; }

    // Sets wheel speed rear right
    void set_DHR(short value){ raw = (raw & 0xffffffffc000ffff) | ((uint64_t)value & 0x3fff) << 16; }
    // Gets wheel speed rear right
    short get_DHR() { return raw >> 16 & 0x3fff; }

    // Sets direction of rotation of rear left wheel
    void set_DRTGHL(uint8_t value){ raw = (raw & 0xffffffffffff3fff) | ((uint64_t)value & 0x3) << 14; }
    // Gets direction of rotation of rear left wheel
    uint8_t get_DRTGHL() { return raw >> 14 & 0x3; }

    // Sets rear left wheel speed
    void set_DHL(short value){ raw = (raw & 0xffffffffffffc000) | ((uint64_t)value & 0x3fff) << 0; }
    // Gets rear left wheel speed
    short get_DHL() { return raw >> 0 & 0x3fff; }

    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == BS_208_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = BS_208_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} BS_208;

#endif
