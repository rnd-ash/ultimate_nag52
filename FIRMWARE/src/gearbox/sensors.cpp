#include "sensors.h"
#include <Arduino.h>
#include "../pins.h"

static int get_voltage_mv() {
    return ((float)analogRead(V_SENSE) / 56.0) * 1000;
}