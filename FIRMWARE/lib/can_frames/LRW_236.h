#ifndef LRW_236_H_
#define LRW_236_H_

#include <stdint.h>
#include <can_c_enums.h>

#define LRW_236_ID 0x0236

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets steering wheel angle
    void set_LRW(short value){ raw = (raw & 0xc000ffffffffffff) | ((uint64_t)value & 0x3fff) << 48; }
    // Gets steering wheel angle
    short get_LRW() { return raw >> 48 & 0x3fff; }

    // Sets steering wheel angular speed
    void set_VLRW(short value){ raw = (raw & 0xffffc000ffffffff) | ((uint64_t)value & 0x3fff) << 32; }
    // Gets steering wheel angular speed
    short get_VLRW() { return raw >> 32 & 0x3fff; }

    // Sets Steering wheel angle sensor status
    void set_LRWS_ST(uint8_t value){ raw = (raw & 0xfffffffffcffffff) | ((uint64_t)value & 0x3) << 24; }
    // Gets Steering wheel angle sensor status
    uint8_t get_LRWS_ST() { return raw >> 24 & 0x3; }

    // Sets Identification of the steering wheel angle sensor
    void set_LRWS_ID(uint8_t value){ raw = (raw & 0xfffffffff3ffffff) | ((uint64_t)value & 0x3) << 26; }
    // Gets Identification of the steering wheel angle sensor
    uint8_t get_LRWS_ID() { return raw >> 26 & 0x3; }

    // Sets message counter
    void set_BZ236h(uint8_t value){ raw = (raw & 0xffffffff0fffffff) | ((uint64_t)value & 0xf) << 28; }
    // Gets message counter
    uint8_t get_BZ236h() { return raw >> 28 & 0xf; }

    // Sets CRC checksum byte 1
    void set_CRC236h(uint8_t value){ raw = (raw & 0xffffffffffffff00) | ((uint64_t)value & 0xff) << 0; }
    // Gets CRC checksum byte 1
    uint8_t get_CRC236h() { return raw >> 0 & 0xff; }

    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == LRW_236_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = LRW_236_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} LRW_236;

#endif
