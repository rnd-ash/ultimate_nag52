#include <esp32_can.h>

CAN_FRAME f;

void setup() {
    Serial.begin(115200);
    CAN0.setCANPins(GPIO_NUM_4, GPIO_NUM_5);
    CAN0.enable();
    CAN0.begin(500000);
}

CAN_FRAME send;
uint8_t bytes_read = 0;
uint8_t read_buffer[11] = {0x00};
void loop() {
    if (Serial.available() != 0) {
        int max_read = std::min(11 - bytes_read, Serial.available());
        Serial.readBytes(&read_buffer[bytes_read], max_read);
        bytes_read += max_read;
    }  

    if (bytes_read == 11) {
        bytes_read = 0;
        send.id = read_buffer[0] << 8 | read_buffer[1];
        send.length = read_buffer[2];
        memcpy(&send.data.bytes[0], &read_buffer[3], send.length);
        Can0.sendFrame(send);

    }
    //CAN0.sendFrame(send);
}
