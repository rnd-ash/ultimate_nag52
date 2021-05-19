#include <Arduino.h>

#include "scn/scn.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial){}
  Serial.println("--NAG52 STARTUP--");
  SCN_CONFIG::ScnConfiguration cfg = SCN_CONFIG::read_scn();

  Serial.println("-- READ SCN FROM EEPROM --\n");

  Serial.print("SCN Variant label: ");
  Serial.println(cfg.variant);

  Serial.print("SCN Checksum OK?: ");
  Serial.println(cfg.cs_ok ? "Yes" : "No");

  Serial.print("SCN Wheel size (mm): ");
  Serial.println(cfg.tire_size_mm);

  Serial.print("SCN Rear differential ratio: ");
  Serial.print(((float)cfg.rear_diff_ratio)/1000.0);
  Serial.println(":1");

  Serial.print("SCN Shift paddles present?: ");
  Serial.println(cfg.has_paddles ? "Yes" : "No");
}

void loop() {
}