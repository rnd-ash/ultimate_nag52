//
// Created by ashcon on 2/20/21.
//

#ifndef ECU_CAN_SIMULATOR_NAG52_H
#define ECU_CAN_SIMULATOR_NAG52_H

#include "../abstract_ecu.h"
#include "../extern_frames.h"

// W5A580 - Max torque 580Nm
// W5A330 - Max torque 330Nm

#define W5A330 // Change to W5A580 for the beefier 722.6 gearbox

#ifdef W5A330
#define D1_RAT 3.9319
#define D2_RAT 2.4079
#define D3_RAT 1.4857
#define D4_RAT 1.0000
#define D5_RAT 0.8305
#define R1_RAT -3.1002
#define R2_RAT -1.8986
#elif W5A580
#define D1_RAT 3.5876
#define D2_RAT 2.1862
#define D3_RAT 1.4054
#define D4_RAT 1.0000
#define D5_RAT 0.8314
#define R1_RAT -3.1605
#define R2_RAT -1.9259
#endif

enum class DriveProgramMode {
    Agility, // Agility mode (faster TC lockup)
    Comfort, // Comfort mode (Starts in second gear)
    Manual, // Manual (tiptronic mode)
    Sport, // Sport (Standard)
    Fail // F marker
};

/*
 * http://ftp.hamsk.ru/auto/W210/AKPP%20Mercedes-Manual.pdf
 *
 * NAG52 controls 5 solenoids:
 * 1. TCC solenoid (PWM)
 * 2. MPC solenoid (PWM)
 * 3. SPC solenoid (PWM)
 * 4. 1-2 & 4-5 solenoid (ON/OFF)
 * 5. 2-3 solenoid (ON/OFF)
 * 6. 3-4 solenoid (ON/OFF)
 *
 * MPC's job is to keep a constant line pressure regardless of the pump speed based on engine RPM
 * SPC's job is to set the pressure for changing gears. Only PWM during gear shifts, else it is off
 * TCC's job is to lock up the torque converter
 * other solenoids are only active during gear shifts.
 *
 * # Some points:
 * 1-2/4-5 solenoid is pulsed during cranking of the engine
 * 3-4 solenoid is pulsed while in park and when EWM module is moving (Garage shifting)
 * MPC pulses at 40% when in park or neutral
 * SPC pulses at 33% in park or neutral
 */

class nag52 : public abstract_ecu {
public:
    void setup();
    void simulate_tick();
private:
    DriveProgramMode prog;
};


#endif //ECU_CAN_SIMULATOR_NAG52_H
