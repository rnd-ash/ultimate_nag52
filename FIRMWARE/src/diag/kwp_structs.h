#ifndef KWP_STRUCTS_H_
#define KWP_STRUCTS_H_

#include <stdint.h>
// Structs used for certain commands

struct DtcState {
    uint8_t _reserved: 4;
    uint8_t readiness: 1;
    uint8_t storage_state: 2;
    uint8_t mil_on: 1;
} __attribute__ ((__packed__));


// 1A 87
struct DcxEcuIdentification {
    uint8_t ecu_origin;
    uint8_t supplier_id;
    uint16_t diag_info;
    uint8_t _reserved;
    uint16_t hw_version;
    uint32_t sw_version : 24;
    char part_number[10];
} __attribute__ ((__packed__));

// 1A 86
struct DcsEcuIdentification {
    char part_number_short[5];
    uint8_t hw_build_ww;
    uint8_t hw_build_yy;
    uint8_t sw_build_ww;
    uint8_t sw_build_yy;
    uint8_t supplier_id;
    uint16_t diag_info;
    uint8_t _reserved;
    uint8_t ecu_production_yy;
    uint8_t ecu_production_mm;
    uint8_t ecu_production_dd;
} __attribute__ ((__packed__));


// --- Used by read data by local ID (SID: 0x21) ---

struct DT_21_31 {
    uint16_t n2_speed; // 1/min
    uint16_t n3_speed; // 1/min
    uint16_t turbine_speed; // 1/min
    uint16_t engine_speed; // 1/min
    uint16_t wheel_fl_spd; // 1/min
    uint16_t wheel_fr_spd; // 1/min
    uint16_t wheel_rl_spd; // 1/min
    uint16_t wheel_rr_spd; // 1/min
    uint16_t spd_rear; // Rear wheel vehicle speed km/h (x10)
    uint16_t spd_front; // Front wheel vehicle speed km/h (x10)
} __attribute__ ((__packed__));


#endif