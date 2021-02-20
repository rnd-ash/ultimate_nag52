//
// Created by ashcon on 2/20/21.
//

#include "esp32_forwarder.h"
#include <fcntl.h>
#include <cstdio>
#include <termios.h>
#include <cstdint>
#include <cstdlib>
#include <unistd.h>
#include <cstring>

esp32_forwarder::esp32_forwarder(char *port) {
    this->fd = open(port, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (this->fd == -1) {
        printf("Unable to open port %s!\n", port);
        return;
    } else {
        fcntl(this->fd, F_SETFL, 0);
        printf("Serial port open! - CAN Frames relaying to network");
    }

    // Set port settings
    struct termios port_settings;
    cfsetispeed(&port_settings, B115200);
    cfsetospeed(&port_settings, B115200);

    port_settings.c_cflag &= ~PARENB;
    port_settings.c_cflag &= ~CSTOPB;
    port_settings.c_cflag &= ~CSIZE;
    port_settings.c_cflag |= CS8;
    tcsetattr(this->fd, TCSANOW, &port_settings);

    // Set port timeouts
    this->timeout.tv_sec = 1;
    this->timeout.tv_usec = 0;
}

void esp32_forwarder::send_frame(CAN_FRAME *f) {
    // We can reduce the number of bytes a bit by using 2 bytes for CANID since W203 network doesn't use extended CAN!
    int num_bytes = 11; // 2 bytes for ID, 1 byte for DLC, 8 bytes for data!
    auto* bytes = static_cast<uint8_t *>(malloc(num_bytes));
    bytes[0] = (f->id >> 8) & 0xFF;
    bytes[1] = (f->id & 0xFF);
    bytes[2] = f->length & 0xFF;
    memcpy(&bytes[3], &f->data.bytes[0], f->length);
    write(this->fd, bytes, num_bytes);
    delete[] bytes;
}

bool esp32_forwarder::is_port_open() const {
    return this->fd != -1;
}

void esp32_forwarder::close_port() {
    close(this->fd);
}
