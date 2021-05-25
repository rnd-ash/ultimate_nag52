/**
 * Pin configuration for Teensy 4.0
 * 
 * PCB Version: 1.2
 */

#ifndef PINS_H_
#define PINS_H_

// Canbus Rx pin
#define CAN_RX 23
// Canbus Tx pin
#define CAN_TX 22
// Voltage sense input
#define V_SENSE 21
// N2 RPM sensor input
#define N2_RPM 20
// ATF Temp and starter lockout sensor input
#define ATF_TEMP 19
// N3 RPM sensor input
#define N3_RPM 18

/**
 * SPI configuration (Solenoid driver ICs)
 */

#define SPI_SCK 13 // SCK
#define SPI_MISO 12 // MISO
#define SPI_MOSI 11 // MOSI
#define SPI_CS_PRESSURE 10 // Chip-select (Pressure solenoid IC)
#define SPI_CS_SHIFT 9 // Chip-select (Shift solenoid IC)

/**
 * PWM Pins for manipulating solenoids
 */

// PWM for Modulating pressure solenoid
#define MPC_PWM 8
// PWM for Torque converter lockup solenoid
#define TCC_PWM 7
// PWM for Shift pressure solenoid
#define SPC_PWM 6
// PWM for 3-4 Shift solenoid
#define Y5_PWM 5
// PWM for 2-3 shift solenoid
#define Y4_PWM 4
// PWM for 1-2/4-5 shift solenoid
#define Y3_PWM 3

/**
 * Solenoid driver ICs reset
 */

#define RESET_PRESSURE 1
#define RESET_SHIFT 0

#endif