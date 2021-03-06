//
// Created by ashcon on 2/20/21.
//

#ifndef ECU_CAN_SIMULATOR_ESP32_FORWARDER_H
#define ECU_CAN_SIMULATOR_ESP32_FORWARDER_H
#include <stdint.h>

struct CAN_FRAME {
    uint32_t id;
    uint8_t dlc;
    uint8_t data[8];

};

#include <time.h>
/**
 * This class handles serial communication with an optional Arduino device running the 'can_forwarder'
 * firmware. This then forwards CAN Frames to a physical CAN Network. In my case, this forwards CAN Frames
 * to IC203
 */
class esp32_forwarder {
public:
    esp32_forwarder();
    esp32_forwarder(char* port);
    void send_frame(CAN_FRAME *f);
    void close_port();
    bool is_port_open() const;
private:
    int fd;
    uint8_t write_buf[11] = {0x00};
    struct timeval timeout;
};


#endif //ECU_CAN_SIMULATOR_ESP32_FORWARDER_H
