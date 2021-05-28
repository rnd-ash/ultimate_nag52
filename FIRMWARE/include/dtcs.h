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

enum class DtcCode {
    /**
     * TCM internal errors
     */
    P2000 = 0x2000, // Control unit watchdog error
    P2004 = 0x2004, // Control unit clock error
    P2005 = 0x2005, // Control unit RAM error
    P2008 = 0x2008, // Control unit ROM error
    P200A = 0x200A, // EEPROM non functional
    P200C = 0x200C, // Control flow error (Kernel)
    P2010 = 0x2010, // Uncoded EGS module (EEPROM blank - no SCN coding present)
    P2011 = 0x2011, // Invalid variant identifier
    P2012 = 0x2012, // SCN Checksum invalid

    /**
     * Gearbox solenoid errors
     */
    P2100 = 0x2100, // Y3 solenoid error
    P2101 = 0x2101, // Y3 solenoid short circuit
    P2102 = 0x2102, // Y4 solenoid error
    P2103 = 0x2103, // Y4 solenoid short circuit
    P2104 = 0x2104, // Y5 solenoid error
    P2105 = 0x2105, // Y5 solenoid short circuit
    P2106 = 0x2106, // TCC solenoid error
    P2107 = 0x2107, // MPC solenoid error
    P2108 = 0x2108, // SPC solenoid error
    P2109 = 0x2109, // R-P Lock solenoid error
    P210A = 0x210A, // Starter lock relay error

    /**
     * Gearbox sensor errors
     */
    P2200 = 0x2200, // N2 speed sensor no signal
    P2203 = 0x2203, // N3 speed sensor no signal
    P2206 = 0x2206, // Downforce speed sensor no signal
    P2207 = 0x2207, // Downforce speed sensor consistency error
    P220A = 0x220A, // N2 and N3 speed sensors disagree
    P220B = 0x220B, // N2 or N3 speed sensors overspeed
    P2210 = 0x2210, // Selector lever SCN coding invalid
    P2211 = 0x2211, // Selector lever position implausible
    P2220 = 0x2220, // ATF Temp sensor / Starter lockout contact short circuit
    P2221 = 0x2221, // ATF Temp sensor / Starter lockout contact implausible
    P2222 = 0x2222, // ATF Temp sensor / Starter lockout contact inconsistent
    P2226 = 0x2226, // Gearbox overheated

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

    P2300 = 0x2300, // CAN Controller error
    P2310 = 0x2310, // CAN communication with BS disturbed
    P2311 = 0x2311, // CAN communication with MS disturbed (short term)
    P2312 = 0x2312, // CAN communication with MS disturbed (long term)
    P2313 = 0x2313, // CAN communication with EWM disturbed
    P2314 = 0x2314, // CAN communication with EZS disturbed
    P2315 = 0x2315, // CAN communication with KOMBI disturbed
    P2316 = 0x2316, // CAN communication with AAC disturbed
    P2317 = 0x2317, // CAN communication with VG disturbed
    P2322 = 0x2322, // CAN: Variant data from MS missing
    P2330 = 0x2330, // CAN message length from BS inconsistent
    P2331 = 0x2331, // CAN message length from MS inconsistent (short term)
    P2332 = 0x2332, // CAN message length from MS inconsistent (long term)
    P2333 = 0x2333, // CAN message length from EWM inconsistent
    P2334 = 0x2334, // CAN message length from EZS inconsistent
    P2335 = 0x2335, // CAN message length from KOMBI inconsistent
    P2336 = 0x2336, // CAN message length from AAC inconsistent
    P2337 = 0x2337, // CAN message length from VG inconsistent
    P233B = 0x233B, // CAN message length for variant type from MS inconsistent

    /**
     * CAN errors - missing or invalid data
     */
    P2400 = 0x2400, // CAN Wheel speed RR (From BS) not avaliable
    P2401 = 0x2401, // CAN Wheel speed RL (From BS) not avaliable
    P2402 = 0x2402, // CAN Wheel speed FR (From BS) not avaliable
    P2403 = 0x2403, // CAN Wheel speed FL (From BS) not avaliable
    P2404 = 0x2404, // CAN Brake light switch (From BS) not avaliable
    P2405 = 0x2405, // CAN Accelerator pedal position (From MS) not avaliable
    P2406 = 0x2406, // CAN engine static torque (From MS) not avaliable
    P2407 = 0x2407, // CAN default torque (From BS) not avaliable
    P2408 = 0x2408, // CAN engine minimal torque (From MS) not avaliable
    P2409 = 0x2409, // CAN engine maximum torque (From MS) not avaliable
    P240A = 0x240A, // CAN engine RPM (From MS) not avaliable
    P240B = 0x240B, // CAN engine coolant temperature (From MS) not avaliable
    P240C = 0x240C, // CAN Selector lever position (From EWM) not avaliable
    P240D = 0x240D, // CAN Transfer case position (From VG) not avaliable
    P2415 = 0x2415, // CAN Steering wheel paddle positions (From MRM) not avaliable
    P2418 = 0x2418, // CAN Steering wheel control element (From MRM) not avaliable
    P2430 = 0x2430, // CAN Multiple wheel speeds (from BS) not avaliable
    P2450 = 0x2450, // CAN Variant ID (From MS) is invalid
    P2451 = 0x2451, // Variant coding - EGS and MS mismatch!

    /**
     * Gearbox related errors (Gear ratios)
     */
    P2500 = 0x2500, // Inadmissible gear ratio
    P2501 = 0x2501, // Engine overspeed
    P2502 = 0x2502, // Gear implausible or transmission is slipping
    P2503 = 0x2503, // Gear comparison implausible multiple times!
    P2507 = 0x2507, // Overspeed N2 RPM sensor
    P2508 = 0x2508, // Overspeed N3 RPM sensor
    P2510 = 0x2510, // Torque converter uncommanded lockup!
    P2511 = 0x2511, // TCC solenoid - Excessive power consumption
    P2512 = 0x2512, // Torque converter control not possible
    P2520 = 0x2520, // Gear protection (multiple times) was not received
    P2560 = 0x2560, // Gear 1 implausible or transmission slipping
    P2561 = 0x2561, // Gear 2 implausible or transmission slipping
    P2562 = 0x2562, // Gear 3 implausible or transmission slipping
    P2563 = 0x2563, // Gear 4 implausible or transmission slipping
    P2564 = 0x2564, // Gear 5 implausible or transmission slipping

    /**
     * Power supply related errors
     */
    P2600 = 0x2600, // Undervoltage to entire module
    P2601 = 0x2601, // Overvoltage to entire power supply
    P2602 = 0x2602, // Supply voltage for module outside tolerance
    P2603 = 0x2603, // Supply voltage for sensors outside tolerance
    P260E = 0x260E, // Supply voltage for sensors undervoltage
    P260F = 0x260F, // Supply voltage for sensors overvoltage

    // ------------------------------------------------------------------------------------- //
    // Codes Below are ultimate-nag52 specific and are not recognised by Daimler's software  //
    // Use OpenVehicleDiagnostic (OVD) for diagnostics with these codes                      //
    // ------------------------------------------------------------------------------------- //

    // 404 - Gearbox not found 
    P0404 = 0x0404, // Where gearbox? (No solenoid control or sensor inputs)
};




// ------------------------------------------------------------- //
// Functions for flagging and clearing error codes from the ECU  //
// ------------------------------------------------------------- //

namespace DTCS {
    bool trigger_dtc(DtcCode dtc);
    uint8_t get_dtc_trigger_count(DtcCode dtc);
    bool clear_dtc(DtcCode dtc);
}

#endif