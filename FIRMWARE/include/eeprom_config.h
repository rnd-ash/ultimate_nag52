/**
 * EEPROM configuration for what is stored in which region of EEPROM
 * 
 * We only have 1024 bytes! (0x0400)
 */

#define EEPROM_MAX_SIZE 0x0400

// Region to dump SCN coding (40 bytes required)
#define SCN_ADDR_START 0x0000
#define SCN_ADDR_END 0x0028

