#ifndef CONFIG_H_
#define CONFIG_H_

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "scn.h"

#define DEFAULT_VIN "00000000000000000" // Default VIN for EEPROM

const uint8_t DEFAULT_SCN[20] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};





/**
 * Static assertions for configuration
 * 
 *  DO NOT DELETE THIS as you might totally break your gearbox!
 */
static_assert(strlen(DEFAULT_VIN) == 17, "VIN is not 17 characters long");


#endif