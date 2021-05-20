#ifndef CONFIG_H_
#define CONFIG_H_

#include <assert.h>
#include <string.h>
#include <stdint.h>

#define HW_VERSION "0001"
#define SW_VERSION_BCD "000100"

#define PART_NUMBER "0345454332"
#define DEFAULT_VIN "00000000000000000" // Default VIN for EEPROM


// ---- ADDITIONAL CONFIGURATION OPTIONS (SCN CODING HAS NO EFFECT ON THESE VALUES) ---- //

// Shipped firmware will have these parameters set to their default

// Forces gearbox to autoshift up on redline in Manual mode (Or range restrict)
// Disabling this will allow you to bounce off the red line in Drive
// This DOES NOT affect downshifting near stall speed
// Default EGS52 behaviour: true
const bool AUTOSHIFT_ON_REDLINE = true;

// Maximum lockup percentage allowed by the torque converter
// 0 - Open clutch
// 100 - Locked clutch
// 1-99 - Slipping clutch
// NOTE This is an ABSOLUTE maximum, and will override the shift programs
// Default EGS52 behaviour: 95
const uint8_t MAX_TCC_LOCK = 100;

// Only effects Agility mode (A). 
// Allows gearbox to downshift before hitting stall speed, but then
// lock up torque converter after.
// This is similar to AMG Speedshift
// Default EGS52 behaviour: false
const bool DYNAMIC_DOWNSHIFT_AGILITY = false;

// Default start gear in Comfort mode
// Winter (W) always starts in 2nd
// S/M/A (Sports, Manual, Agility) always starts in 1st
// Comfort (C) is adjustable with this value
// Default EGS52 behaviour: true (Start in 2nd)
const bool C_START_2ND = true;

// CAUTION: Modifying this might mechanically damage your gearbox long term!
// 
// If disabled, maximum shift pressure is always applied, which results in super fast gear changes,
// but might feel like a train impacting your car when changing under low engine loads.
// Default EGS52 behaviour: true (Take care of the gearbox)
const bool I_CARE_ABOUT_MY_GEARBOX = true;



// ------------------------------------------ //






/**
 * Static assertions for configuration
 * 
 *  DO NOT DELETE THIS as you might totally break your gearbox!
 */
static_assert(strlen(DEFAULT_VIN) == 17, "VIN is not 17 characters long");
static_assert(strlen(PART_NUMBER) == 10, "Part number is not 10 characters long");

#endif