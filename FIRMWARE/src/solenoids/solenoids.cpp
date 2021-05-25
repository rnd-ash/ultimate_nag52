#include "solenoids.h"
#include "../pins.h"
#include <Arduino.h>
#include "SPI.h"

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
    delayMicroseconds(15);
    digitalWrite(RESET_SHIFT, HIGH);
    digitalWrite(RESET_PRESSURE, HIGH);
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

    //uint8_t shift_amount = 0;
    //switch (s) {
    //    case Solenoid::TCC:
    //    case Solenoid::Y3:
    //        // Channel 1
    //        shift_amount = 0;
    //        break;
    //    case Solenoid::SPC:
    //    case Solenoid::Y4:
    //         // Channel 2
    //        shift_amount = 2;
    //        break;
    //    case Solenoid::MPC:
    //    case Solenoid::Y5:
    //         // Channel 3
    //        shift_amount = 4;
    //        break;
    //    default:
    //        break;
    //}
    //return (SolenoidState)((res >> shift_amount) & 0b11);
}

void SolenoidControl::set_pwm(Solenoid s, uint8_t pwm) {
    uint8_t val = 255-pwm; // Ver 1.x PCB has PRG shorted to ground, meaning 255 is off, 0 is on
    switch (s) {
        case Solenoid::Y3:
            analogWrite(Y3_PWM, val);
            this->pwm_y3 = val;
            break;
        case Solenoid::Y4:
            analogWrite(Y4_PWM, val);
            this->pwm_y4 = val;
            break;
        case Solenoid::Y5:
            analogWrite(Y5_PWM, val);
            this->pwm_y5 = val;
            break;
        case Solenoid::SPC:
            analogWrite(SPC_PWM, val);
            this->pwm_spc = val;
            break;
        case Solenoid::MPC:
            analogWrite(MPC_PWM, val);
            this->pwm_mpc = val;
            break;
        case Solenoid::TCC:
            analogWrite(TCC_PWM, val);
            this->pwm_tcc = val;
            break;
        default:
            break;
    }

}

uint8_t SolenoidControl::get_pwm(Solenoid s) {
    switch (s) {
        case Solenoid::Y3:
            return 255-this->pwm_y3;
        case Solenoid::Y4:
            return 255-this->pwm_y4;
        case Solenoid::Y5:
            return 255-this->pwm_y5;
        case Solenoid::SPC:
            return 255-this->pwm_spc;
        case Solenoid::MPC:
            return 255-this->pwm_mpc;
        case Solenoid::TCC:
            return 255-this->pwm_tcc;
        default:
            break;
    }
    return 0;
}

uint8_t SolenoidControl::spi_transfer(uint8_t req, uint8_t cs_pin) {
    SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE0)); // 5Mhz for TLE6220GP
    digitalWrite(cs_pin, LOW);
    uint8_t res = SPI.transfer(req);
    digitalWrite(cs_pin, HIGH);
    SPI.endTransaction();
    return res;
}

