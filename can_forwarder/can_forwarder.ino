#include <esp32_can.h>

CAN_FRAME f;
void setup() {
    Serial.begin(115200);
    CAN0.setCANPins(GPIO_NUM_4, GPIO_NUM_5);
    CAN0.enable();
    CAN0.begin(500000);
}

CAN_FRAME send;
uint8_t read_buffer[11] = {0x00};
void loop() {
    if (Serial.available() >= 11) {
        Serial.readBytes(&read_buffer[0], 11);
        send.id = 0x0000 | (read_buffer[0] << 8) | read_buffer[1];
        send.length = read_buffer[2];
        memcpy(&send.data.bytes[0], &read_buffer[3], send.length);
        CAN0.sendFrame(send);
    }
}
