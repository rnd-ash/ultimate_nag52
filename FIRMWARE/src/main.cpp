#include <Arduino.h>
#include <FreeRTOS_TEENSY4.h>
#include "diag/kwp2000.h"
#include "solenoids/solenoids.h"
#include "can_network/can_manager.h"
#include "pins.h"

//FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> canc;

static void SolenoidServer(void* args) {
  SolenoidControl s = SolenoidControl();
  unsigned long last_update = millis();
  uint16_t x = 0;
  s.set_current(Solenoid::TCC, 2000);
  while(1) {
    //s.update();
    //analogWrite(TCC_PWM, 0);
    //vTaskDelay(500);
    //analogWrite(TCC_PWM, 256);
    vTaskDelay(500);
    //Serial.print(x);
    //Serial.print(" ");
    //Serial.print(s.get_pwm(Solenoid::TCC));
    //Serial.print(" ");
    //Serial.println(s.get_current(Solenoid::TCC));
    //if (millis() - last_update > 2000) {
    //  last_update = millis();
    //  Serial.println(s.get_pwm(Solenoid::TCC));
    //  Serial.print("TCC current: ");
    //  Serial.println(s.get_current(Solenoid::TCC));
    //  s.query_solenoid_states();
    //  if (s.get_solenoid_state(Solenoid::TCC) != SolenoidState::OSO) {
    //    s.set_current(Solenoid::TCC, 1000);
    //  } else {
    //    s.set_current(Solenoid::TCC, 0);
    //    Serial.println("WARNING TCC OVERHEAT!");
    //  }
    //  Serial.println((int)s.get_solenoid_state(Solenoid::TCC));
    //}
  }
}


void task_creation_error_loop() {
  pinMode(13, OUTPUT);
  while(1) {
    delay(500);
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
  }
}

void rpm_beat() {
  Serial.println("RPM Sensor!");
}

void setup() {
  while(!Serial){}
  // put your setup code here, to run once:
  Serial.begin(9600);
  init_can();
  pinMode(N3_RPM, INPUT_PULLUP);
  pinMode(N2_RPM, INPUT_PULLUP);
  pinMode(ATF_TEMP, INPUT);
  attachInterrupt(digitalPinToInterrupt(N3_RPM), rpm_beat, FALLING);

  KWPDiagServer s = KWPDiagServer(kwp2000_iso_tp_server);

  xTaskCreate(SolenoidServer, "Solenoid controller", 4096, NULL, 5, NULL);
  vTaskStartScheduler();
}

void loop() {
}