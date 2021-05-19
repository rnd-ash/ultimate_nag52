/**
 * EEPROM configuration for what is stored in which region of EEPROM
 * 
 * We only have 1024 bytes! (0x0400)
 */

#define EEPROM_MAX_SIZE 0x0400

// Need byte 0 for something (Not sure yet)

// Region to store SCN coding (40 bytes required) - Includes checksum
#define SCN_ADDR_START 1
#define SCN_ADDR_END SCN_ADDR_START+40

// Region to store vehicles VIN number (17 ASCII characters)
#define VIN_ADDR_START SCN_ADDR_END+1
#define VIN_ADDR_END VIN_ADDR_START+17