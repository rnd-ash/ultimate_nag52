#include "solenoids.h"
#include "../pins.h"
#include <Arduino.h>
#include "SPI.h"
#include "../gearbox/sensors.h"

SolenoidControl::SolenoidControl() {
    SPI.begin();
    pinMode(SPI_CS_SHIFT, OUTPUT);
    pinMode(SPI_CS_PRESSURE, OUTPUT);
    pinMode(RESET_SHIFT, OUTPUT);
    pinMode(RESET_PRESSURE, OUTPUT);

    // Write 2 CS pins to high
    digitalWrite(SPI_CS_PRESSURE, HIGH);
    digitalWrite(SPI_CS_SHIFT, HIGH);

    // Activate reset (15us)
    digitalWrite(RESET_SHIFT, LOW);
    digitalWrite(RESET_PRESSURE, LOW);
    digitalWrite(RESET_SHIFT, HIGH);
    digitalWrite(RESET_PRESSURE, HIGH);

    // Now set all solenoid pins to output
    pinMode(TCC_PWM, OUTPUT);
    pinMode(SPC_PWM, OUTPUT);
    pinMode(MPC_PWM, OUTPUT);
    pinMode(Y3_PWM, OUTPUT);
    pinMode(Y4_PWM, OUTPUT);
    pinMode(Y5_PWM, OUTPUT);

    //analogWriteResolution(12);
    analogWriteFrequency(TCC_PWM, 100); // 100Hz for TCC
    analogWriteFrequency(MPC_PWM, 1000); // 1000Hz for MPC
    analogWriteFrequency(SPC_PWM, 1000); // 1000Hz for SPC

    // All solenoids off by default!
    analogWrite(TCC_PWM, 256);
    analogWrite(SPC_PWM, 256);
    analogWrite(MPC_PWM, 256);
    analogWrite(Y3_PWM, 256);
    analogWrite(Y4_PWM, 256);
    analogWrite(Y5_PWM, 256);
}

SolenoidState SolenoidControl::get_solenoid_state(Solenoid s) {
    switch(s) {
        case Solenoid::Y3:
            return this->shift_states[0];
        case Solenoid::Y4:
            return this->shift_states[1];
        case Solenoid::Y5:
            return this->shift_states[2];
        case Solenoid::TCC:
            return this->pressure_states[0];
        case Solenoid::SPC:
            return this->pressure_states[1];
        case Solenoid::MPC:
            return this->pressure_states[2];
        default:
            break;
    }
    return SolenoidState::Unknown;
}

void SolenoidControl::query_solenoid_states() {
    uint8_t shift_status = this->spi_transfer(0B00000000, SPI_CS_SHIFT);
    uint8_t pressure_status = this->spi_transfer(0B00000000, SPI_CS_PRESSURE);

    // Channel 1
    this->pressure_states[0] = (SolenoidState)(pressure_status & 0x03);
    this->shift_states[0] = (SolenoidState)(shift_status & 0x03);

    // Channel 2
    this->pressure_states[1] = (SolenoidState)((pressure_status >> 2) & 0x03);
    this->shift_states[1] = (SolenoidState)((shift_status >> 2) & 0x03);

    // Channel 3
    this->pressure_states[2] = (SolenoidState)((pressure_status >> 4) & 0x03); 
    this->shift_states[2] = (SolenoidState)((shift_status >> 4) & 0x03);
}

void SolenoidControl::set_current(Solenoid s, int current) {
    solenoid_powers[(uint8_t)s].targ_current = current;
}

uint8_t SolenoidControl::get_pwm(Solenoid s) {
    return solenoid_powers[(uint8_t)s].pwm_current;
}

int SolenoidControl::get_current(Solenoid s) {
    uint8_t pwm = solenoid_powers[(uint8_t)s].pwm_current;
    float resistance = 4.5;
    if (s == Solenoid::TCC) {
        resistance = 1;
    } else if (s == Solenoid::MPC || s == Solenoid::SPC) {
        resistance = 6;
    }
    return (int)((float)get_voltage_mv()*((float)pwm/256.0)/resistance);
}

void SolenoidControl::update() {
    update_solenoid(Solenoid::TCC);
    //update_solenoid(Solenoid::MPC);
    //update_solenoid(Solenoid::SPC);
    //update_solenoid(Solenoid::Y3);
    //update_solenoid(Solenoid::Y4);
    //update_solenoid(Solenoid::Y5);
}

void SolenoidControl::update_solenoid(Solenoid s) {
    // How much is the solenoid currently drawing?
    SolenoidControlState* sol_state = &solenoid_powers[(uint8_t)s];
    float v = (float)get_voltage_mv();
    float resistance = 4.5;
    if (s == Solenoid::TCC) {
        resistance = 1;
    } else if (s == Solenoid::MPC || s == Solenoid::SPC) {
        resistance = 6;
    }

    // How much current do we want?
    float target_voltage = (float)(sol_state->targ_current)*resistance;
    float ratio = target_voltage / v;
    float pwm_f = (256.0 * ratio);
    uint8_t pwm = max(min(256, (uint8_t)pwm_f), 0);
    sol_state->pwm_current = pwm;
    switch(s) {
    case Solenoid::Y3:
        analogWrite(Y3_PWM, 256-pwm);
        break;
    case Solenoid::Y4:
        analogWrite(Y4_PWM, 256-pwm);
        break;
    case Solenoid::Y5:
        analogWrite(Y5_PWM, 256-pwm);
        break;
    case Solenoid::TCC:
        analogWrite(TCC_PWM, 256-pwm);
        break;
    case Solenoid::SPC:
        analogWrite(SPC_PWM, 256-pwm);
        break;
    case Solenoid::MPC:
        analogWrite(MPC_PWM, 256-pwm);
        break;
    default:
        break;
    }
}

uint8_t SolenoidControl::spi_transfer(uint8_t req, uint8_t cs_pin) {
    SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE0)); // 5Mhz for TLE6220GP
    digitalWrite(cs_pin, LOW);
    delayNanoseconds(300); // Teensy is too fast. Wait 250ns for TLE6220GP to be ready
    uint8_t res = SPI.transfer(req);
    digitalWrite(cs_pin, HIGH);
    SPI.endTransaction();
    return res;
}

