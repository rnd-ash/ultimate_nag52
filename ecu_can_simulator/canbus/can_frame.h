//
// Created by ashcon on 2/20/21.
//

#ifndef ECU_CAN_SIMULATOR_CAN_FRAME_H
#define ECU_CAN_SIMULATOR_CAN_FRAME_H

#include <stdint.h>

// From CAN_COMMON (Collin80)
class BitRef {
public:
    BitRef& operator=( bool x )
    {
        *byteRef = (*byteRef & ~(1 << bitPos));
        if (x) *byteRef = *byteRef | (1 << bitPos);
        return *this;
    }
    //BitRef& operator=( const BitRef& x );

    operator bool() const
    {
        if (*byteRef & (1 << bitPos)) return true;
        return false;
    }
public:
    BitRef(uint8_t *ref, int pos)
    {
        byteRef = ref;
        bitPos = pos;
    }
private:
    uint8_t *byteRef;
    int bitPos;
};


typedef union {
    uint64_t uint64{};
    uint32_t uint32[2];
    uint16_t uint16[4];
    uint8_t  uint8[8];
    int64_t int64;
    int32_t int32[2];
    int16_t int16[4];
    int8_t  int8[8];

    //deprecated names used by older code
    uint64_t value;
    struct {
        uint32_t low;
        uint32_t high;
    };
    struct {
        uint16_t s0;
        uint16_t s1;
        uint16_t s2;
        uint16_t s3;
    };
    uint8_t bytes[8];
    uint8_t byte[8]; //alternate name so you can omit the s if you feel it makes more sense
    struct {
        uint8_t bitField[8];
        bool operator[]( int pos ) const
        {
            if (pos < 0 || pos > 63) return 0;
            int bitFieldIdx = pos / 8;
            return (bitField[bitFieldIdx] >> pos) & 1;
        }
        BitRef operator[]( int pos )
        {
            if (pos < 0 || pos > 63) return BitRef((uint8_t *)&bitField[0], 0);
            uint8_t *ptr = (uint8_t *)&bitField[0];
            return BitRef(ptr + (pos / 8), pos & 7);
        }
    } bit;
} BytesUnion;


class CAN_FRAME
{
public:
    CAN_FRAME() {
        id = 0;
        fid = 0;
        rtr = 0;
        priority = 15;
        extended = false;
        timestamp = 0;
        length = 0;
        data.value = 0;
    }

    BytesUnion data;      // 64 bits - lots of ways to access it.
    uint32_t id{};        // 29 bit if ide set, 11 bit otherwise
    uint32_t fid{};       // family ID - used internally to library
    uint32_t timestamp{}; // CAN timer value when mailbox message was received.
    uint8_t rtr{};        // Remote Transmission Request (1 = RTR, 0 = data frame)
    uint8_t priority{};   // Priority but only important for TX frames and then only for special uses (0-31)
    uint8_t extended{};   // Extended ID flag
    uint8_t length{};     // Number of data bytes
};

#endif //ECU_CAN_SIMULATOR_CAN_FRAME_H
