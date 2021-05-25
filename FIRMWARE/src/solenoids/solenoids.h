#ifndef SOLENOIDS_H_
#define SOLENOIDS_H_

#include <stdint.h>

enum class Solenoid {
    Y3,
    Y4,
    Y5,
    SPC,
    MPC,
    TCC
};

enum class SolenoidState {
    Unknown = 4,
    Normal = 3,
    OSO = 2,
    OpenLoad = 1,
    ShortedGround = 0,
};

class SolenoidControl {
public:
    SolenoidControl();
    void query_solenoid_states();
    SolenoidState get_solenoid_state(Solenoid s);
    // PWM means 0 = off, 255 = on
    void set_pwm(Solenoid s, uint8_t pwm);
    uint8_t get_pwm(Solenoid s);
private:
    uint8_t spi_transfer(uint8_t req, uint8_t cs_pin);
    uint8_t pwm_spc = 0;
    uint8_t pwm_mpc = 0;
    uint8_t pwm_tcc = 0;

    uint8_t pwm_y3 = 0;
    uint8_t pwm_y4 = 0;
    uint8_t pwm_y5 = 0;

    SolenoidState pressure_states[3] = {SolenoidState::Unknown, SolenoidState::Unknown, SolenoidState::Unknown};
    SolenoidState shift_states[3] = {SolenoidState::Unknown, SolenoidState::Unknown, SolenoidState::Unknown};
};

#endif