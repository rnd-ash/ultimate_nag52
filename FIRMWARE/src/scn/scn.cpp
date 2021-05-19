#include <EEPROM.h>

#include "scn.h"
#include <eeprom_config.h>
#include <string.h>

namespace SCN_CONFIG {
    ScnConfiguration read_scn() {
        uint8_t read_buffer[SCN_BYTE_LENGTH];
        for (int i = 0; i < SCN_BYTE_LENGTH; i++) {
            read_buffer[i] = EEPROM.read(SCN_ADDR_START + i);
        }

        ScnConfiguration cfg;
        memset(&cfg, 0x00, sizeof(cfg));

        // TODO Check checksum
        cfg.cs_ok = true;

        memcpy(&cfg.variant[0], &read_buffer[0], 4);

        cfg.tire_size_mm =    (uint16_t)read_buffer[34] | read_buffer[35] << 8;
        cfg.rear_diff_ratio = (uint16_t)read_buffer[30] | read_buffer[31] << 8;
        cfg.has_paddles = read_buffer[28] & 0b00100000;

        cfg.has_transfer_case = false;

        if (cfg.has_transfer_case) {
            cfg.transfer_case_ratio_low = 0;
            cfg.transfer_case_ratio_high = 0;
        }

        return cfg;
    }
}