#ifndef GS_419_H_
#define GS_419_H_

#include <stdint.h>

/**
 * This CAN Frame is a custom one specific to Ultimate NAG52.
 * You will not find it in standard MB vehicles.
 *
 * This CAN Frame is used for debugging and contains data
 * about the current solenoid application. Specifically:
 * TCC PWM value
 * MPC PWM value
 * SPC PWM value
 * Y3 PWM value
 * Y4 PWM value
 * Y5 PWM value
 * Shift harshness request (Only during shifts), Otherwise 0
 * Shift compensation (Due to sloppy transmission), again only during shifts, otherwise 0
 */

#define GS_419_ID 0x0419

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets solenoid application of TCC
    void set_TCC_PWM(uint8_t value){ raw = (raw & 0x00ffffffffffffff) | ((uint64_t)value & 0xff) << 56; }
    // Gets solenoid application of TCC
    uint8_t get_TCC_PWM() { return (uint8_t)(raw >> 56 & 0xff); }

    // Sets solenoid application of MPC
    void set_MPC_PWM(uint8_t value){ raw = (raw & 0xff00ffffffffffff) | ((uint64_t)value & 0xff) << 48; }
    // Gets solenoid application of MPC
    uint8_t get_MPC_PWM() { return (uint8_t)(raw >> 48 & 0xff); }

    // Sets solenoid application of SPC
    void set_SPC_PWM(uint8_t value){ raw = (raw & 0xffff00ffffffffff) | ((uint64_t)value & 0xff) << 40; }
    // Gets solenoid application of SPC
    uint8_t get_SPC_PWM() { return (uint8_t)(raw >> 40 & 0xff); }

    // Sets solenoid application of Y3 (1-2 & 4-5 shift)
    void set_Y3_PWM(uint8_t value){ raw = (raw & 0xffffff00ffffffff) | ((uint64_t)value & 0xff) << 32; }
    // Gets solenoid application of Y3 (1-2 & 4-5 shift)
    uint8_t get_Y3_PWM() { return (uint8_t)(raw >> 32 & 0xff); }

    // Sets solenoid application of Y4 (2-3 shift)
    void set_Y4_PWM(uint8_t value){ raw = (raw & 0xffffffff00ffffff) | ((uint64_t)value & 0xff) << 24; }
    // Gets solenoid application of Y4 (2-3 shift)
    uint8_t get_Y4_PWM() { return (uint8_t)(raw >> 24 & 0xff); }

    // Sets solenoid application of Y5 (3-4 shift)
    void set_Y5_PWM(uint8_t value){ raw = (raw & 0xffffffffff00ffff) | ((uint64_t)value & 0xff) << 16; }
    // Gets solenoid application of Y5 (3-4 shift)
    uint8_t get_Y5_PWM() { return (uint8_t)(raw >> 16 & 0xff); }

    // Sets shift harshness request (Harsher = quicker shift)
    void set_SHIFT_HARSHNESS(uint8_t value){ raw = (raw & 0xffffffffffff00ff) | ((uint64_t)value & 0xff) << 8; }
    // Gets shift harshness
    uint8_t get_SHIFT_HARSHNESS() { return (uint8_t)(raw >> 8 & 0xff); }

    // Sets shift compensation value (Sloppy transmission = more compensation)
    void set_SHIFT_COMPENSATION(uint8_t value){ raw = (raw & 0xffffffffffffff00) | ((uint64_t)value & 0xff) << 0; }
    // Gets shift compensation value
    uint8_t get_SHIFT_COMPENSATION() { return (uint8_t)(raw >> 0 & 0xff); }


    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == GS_419_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = GS_419_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} GS_419;

#endif
