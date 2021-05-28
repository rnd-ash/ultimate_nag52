#ifndef SOLENOIDS_H_
#define SOLENOIDS_H_

#include <stdint.h>

// https://www.sonnax.com/tech_resources/279-722-6-solenoid-identification
// Using average resistance values
#define RESISTANCE_TCC 3.0
#define RESISTANCE_MPC 6
#define RESISTANCE_SPC 6
#define RESISTANCE_Y3 4.5
#define RESISTANCE_Y4 4.5
#define RESISTANCE_Y5 4.5

#define TARGET_VOLTAGE 12000 // 12V for solenoids

enum class Solenoid {
    Y3 = 0,
    Y4 = 1,
    Y5 = 2,
    SPC = 3,
    MPC = 4,
    TCC = 5
};

enum class SolenoidState {
    Unknown = 4,
    Normal = 3,
    OSO = 2,
    OpenLoad = 1,
    ShortedGround = 0,
};

struct SolenoidControlState {
    uint8_t pwm_current;
    int targ_current;
};

class SolenoidControl {
public:
    SolenoidControl();
    void query_solenoid_states();
    SolenoidState get_solenoid_state(Solenoid s);
    // PWM means 0 = off, 255 = on
    void set_current(Solenoid s, int current);
    int get_current(Solenoid s);
    uint8_t get_pwm(Solenoid s);
    void update();
private:
    uint8_t spi_transfer(uint8_t req, uint8_t cs_pin);
    
    SolenoidControlState solenoid_powers[6] = {
        {0,0},
        {0,0},
        {0,0},
        {0,0},
        {0,0},
        {0,0}
    };


    SolenoidState pressure_states[3] = {SolenoidState::Unknown, SolenoidState::Unknown, SolenoidState::Unknown};
    SolenoidState shift_states[3] = {SolenoidState::Unknown, SolenoidState::Unknown, SolenoidState::Unknown};

    void update_solenoid(Solenoid s);
};

#endif