#include <esp32_can.h>
#include "A0.h"
#include <can_common.h>
#include "canframes/GS_218.h"

CAN_FRAME f;
void setup() {
    Serial.begin(115200);
    CAN0.setCANPins(CAN0_RX, CAN0_TX);
    CAN0.enable();
    CAN0.begin(500000);
    f.id = 0x0418;
    f.length = 8;
}

void loop() {
    CAN0.sendFrame(f);
    digitalWrite(13, HIGH);
}