#ifndef DTCS_H_
#define DTCS_H_

#include <stdint.h>

/**
 * DTCs (Diagnostic trouble codes) for stock EGS52.
 * 
 * Not all DTCs will be utilized by ultimate-nag52
 * 
 * Diagversion 51
 */

/**
 * TCM internal errors
 */
#define P2000 0x2000 // Control unit watchdog error
#define P2004 0x2004 // Control unit clock error
#define P2005 0x2005 // Control unit RAM error
#define P2008 0x2008 // Control unit ROM error
#define P200A 0x200A // EEPROM non functional
#define P200C 0x200C // Control flow error (Kernel)
#define P2010 0x2010 // Uncoded EGS module (EEPROM blank - no SCN coding present)
#define P2011 0x2011 // Invalid variant identifier
#define P2012 0x2012 // SCN Checksum invalid

/**
 * Gearbox solenoid errors
 */
#define P2100 0x2100 // Y3 solenoid error
#define P2101 0x2101 // Y3 solenoid short circuit
#define P2102 0x2102 // Y4 solenoid error
#define P2103 0x2103 // Y4 solenoid short circuit
#define P2104 0x2104 // Y5 solenoid error
#define P2105 0x2105 // Y5 solenoid short circuit
#define P2106 0x2106 // TCC solenoid error
#define P2107 0x2107 // MPC solenoid error
#define P2108 0x2108 // SPC solenoid error
#define P2109 0x2109 // R-P Lock solenoid error
#define P210A 0x210A // Starter lock relay error

/**
 * Gearbox sensor errors
 */
#define P2200 0x2200 // N2 speed sensor no signal
#define P2203 0x2203 // N3 speed sensor no signal
#define P2206 0x2206 // Downforce speed sensor no signal
#define P2207 0x2207 // Downforce speed sensor consistency error
#define P220A 0x220A // N2 and N3 speed sensors disagree
#define P220B 0x220B // N2 or N3 speed sensors overspeed
#define P2210 0x2210 // Selector lever SCN coding invalid
#define P2211 0x2211 // Selector lever position implausible
#define P2220 0x2220 // ATF Temp sensor / Starter lockout contact short circuit
#define P2221 0x2221 // ATF Temp sensor / Starter lockout contact implausible
#define P2222 0x2222 // ATF Temp sensor / Starter lockout contact inconsistent
#define P2226 0x2226 // Gearbox overheated

/**
 * CAN errors - module communication
 * 
 * ECU Names
 * * BS - ABS and ESP module
 * * MS - Engine ECU
 * * EWM - Shift selector lever
 * * KOMBI - Instrument cluster
 * * AAC - Climate control
 * * VG - Transfer case
 * * EZS - Ignition switch and CANB<->CANC gateway
 * * MRM - Steering wheel and steering wheel columns/buttons
 */

#define P2300 0x2300 // CAN Controller error
#define P2310 0x2310 // CAN communication with BS disturbed
#define P2311 0x2311 // CAN communication with MS disturbed (short term)
#define P2312 0x2312 // CAN communication with MS disturbed (long term)
#define P2313 0x2313 // CAN communication with EWM disturbed
#define P2314 0x2314 // CAN communication with EZS disturbed
#define P2315 0x2315 // CAN communication with KOMBI disturbed
#define P2316 0x2316 // CAN communication with AAC disturbed
#define P2317 0x2317 // CAN communication with VG disturbed
#define P2322 0x2322 // CAN: Variant data from MS missing
#define P2330 0x2330 // CAN message length from BS inconsistent
#define P2331 0x2331 // CAN message length from MS inconsistent (short term)
#define P2332 0x2332 // CAN message length from MS inconsistent (long term)
#define P2333 0x2333 // CAN message length from EWM inconsistent
#define P2334 0x2334 // CAN message length from EZS inconsistent
#define P2335 0x2335 // CAN message length from KOMBI inconsistent
#define P2336 0x2336 // CAN message length from AAC inconsistent
#define P2337 0x2337 // CAN message length from VG inconsistent
#define P233B 0x233B // CAN message length for variant type from MS inconsistent

/**
 * CAN errors - missing or invalid data
 */
#define P2400 0x2400 // CAN Wheel speed RR (From BS) not avaliable
#define P2401 0x2401 // CAN Wheel speed RL (From BS) not avaliable
#define P2402 0x2402 // CAN Wheel speed FR (From BS) not avaliable
#define P2403 0x2403 // CAN Wheel speed FL (From BS) not avaliable
#define P2404 0x2404 // CAN Brake light switch (From BS) not avaliable
#define P2405 0x2405 // CAN Accelerator pedal position (From MS) not avaliable
#define P2406 0x2406 // CAN engine static torque (From MS) not avaliable
#define P2407 0x2407 // CAN default torque (From BS) not avaliable
#define P2408 0x2408 // CAN engine minimal torque (From MS) not avaliable
#define P2409 0x2409 // CAN engine maximum torque (From MS) not avaliable
#define P240A 0x240A // CAN engine RPM (From MS) not avaliable
#define P240B 0x240B // CAN engine coolant temperature (From MS) not avaliable
#define P240C 0x240C // CAN Selector lever position (From EWM) not avaliable
#define P240D 0x240D // CAN Transfer case position (From VG) not avaliable
#define P2415 0x2415 // CAN Steering wheel paddle positions (From MRM) not avaliable
#define P2418 0x2418 // CAN Steering wheel control element (From MRM) not avaliable
#define P2430 0x2430 // CAN Multiple wheel speeds (from BS) not avaliable
#define P2450 0x2450 // CAN Variant ID (From MS) is invalid
#define P2451 0x2451 // Variant coding - EGS and MS mismatch!

/**
 * Gearbox related errors (Gear ratios)
 */
#define P2500 0x2500 // Inadmissible gear ratio
#define P2501 0x2501 // Engine overspeed
#define P2502 0x2502 // Gear implausible or transmission is slipping
#define P2503 0x2503 // Gear comparison implausible multiple times!
#define P2507 0x2507 // Overspeed N2 RPM sensor
#define P2508 0x2508 // Overspeed N3 RPM sensor
#define P2510 0x2510 // Torque converter uncommanded lockup!
#define P2511 0x2511 // TCC solenoid - Excessive power consumption
#define P2512 0x2512 // Torque converter control not possible
#define P2520 0x2520 // Gear protection (multiple times) was not received
#define P2560 0x2560 // Gear 1 implausible or transmission slipping
#define P2561 0x2561 // Gear 2 implausible or transmission slipping
#define P2562 0x2562 // Gear 3 implausible or transmission slipping
#define P2563 0x2563 // Gear 4 implausible or transmission slipping
#define P2564 0x2564 // Gear 5 implausible or transmission slipping

/**
 * Power supply related errors
 */
#define P2600 0x2600 // Undervoltage to entire module
#define P2601 0x2601 // Overvoltage to entire power supply
#define P2602 0x2602 // Supply voltage for module outside tolerance
#define P2603 0x2603 // Supply voltage for sensors outside tolerance
#define P260E 0x260E // Supply voltage for sensors undervoltage
#define P260F 0x260F // Supply voltage for sensors overvoltage


bool trigger_dtc(uint16_t dtc);
uint8_t get_dtc_trigger_count(uint16_t dtc);
bool clear_dtc(uint16_t dtc);


#endif