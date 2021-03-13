#define FW_MODE

#include <esp32_can.h>
#include "firmware/A0.h"
#include <can_common.h>

#include "gearbox_brain.h"

Gearbox *vnag;

void setup() {
    Serial.begin(115200);
    CAN0.setCANPins(CAN0_RX, CAN0_TX);
    CAN0.enable();
    CAN0.begin(500000);
    vnag = new Gearbox();
}

void loop() {
    
}