#ifndef SCN_H_
#define SCN_H_

// From SCN String, we only need the following values,
// the rest are modified in include/CONFIG.h
// 
// Rear axle ratio (Differential)
// Wheel radius (mm)
// Check SCN Checksum is valid

#define SCN_BYTE_LENGTH 40

namespace SCN_CONFIG {
    struct ScnConfiguration {
        char variant[5]; // 4 letter ID with null terminator
        bool cs_ok;
        uint16_t tire_size_mm;
        uint16_t rear_diff_ratio;
        bool has_paddles;
        bool has_transfer_case;
        uint16_t transfer_case_ratio_low;
        uint16_t transfer_case_ratio_high; 
    };

    ScnConfiguration read_scn();
}

#endif