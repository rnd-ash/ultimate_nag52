//
// Created by ashcon on 2/20/21.
//

#include "esp32_forwarder.h"
#include "car_sim.h"
#include <fcntl.h>
#include <cstdio>
#include <termios.h>
#include <cstdint>
#include <unistd.h>
#include <cstring>
#include <sys/ioctl.h>
#include <poll.h>

esp32_forwarder::esp32_forwarder(char *port) {
    this->fd = open(port, O_RDWR | O_NOCTTY);
    if (this->fd == -1) {
        printf("Unable to open port %s!\n", port);
        return;
    } else {
        fcntl(this->fd, F_SETFL, 0);
        printf("Serial port open! - CAN Frames relaying to network\n");
    }

    // Set port settings
    struct termios port_settings;
    if(tcgetattr(this->fd, &port_settings) != 0) {
        printf("Error getting port settings!\n");
        close_port();
        return;
    }

    port_settings.c_cflag &= ~PARENB;
    port_settings.c_cflag &= ~CSTOPB;
    port_settings.c_cflag &= ~CSIZE;
    port_settings.c_cflag |= CS8;
    port_settings.c_cflag &= ~CRTSCTS;
    port_settings.c_cflag |= CREAD | CLOCAL;
    port_settings.c_lflag &= ~ICANON;
    port_settings.c_lflag &= ~ECHO; // Disable echo
    port_settings.c_lflag &= ~ECHOE; // Disable erasure
    port_settings.c_lflag &= ~ECHONL; // Disable new-line echo
    port_settings.c_lflag &= ~ISIG;

    port_settings.c_iflag &= ~(IXON | IXOFF | IXANY);
    port_settings.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
    port_settings.c_oflag &= ~OPOST;
    port_settings.c_oflag &= ~ONLCR;
    port_settings.c_cc[VTIME] = 0;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    port_settings.c_cc[VMIN] = 0;


    cfsetspeed(&port_settings, 115200);
    //port_settings.c_cflag &= ~CBAUD;
    //port_settings.c_cflag |= CBAUDEX;
    //port_settings.c_ispeed = 9600;
    //port_settings.c_ospeed = 9600;

    tcflush(this->fd, TCIFLUSH);
    if((tcsetattr(this->fd,TCSANOW,&port_settings)) != 0){       // Set the attributes to the termios structure
        printf("Error while setting attributes \n");
        close_port();
        return;
    }
}

void esp32_forwarder::send_frame(CAN_FRAME *f) {
    // We can reduce the number of bytes a bit by using 2 bytes for CANID since W203 network doesn't use extended CAN!
    this->write_buf[0] = (uint8_t)(f->id >> 8) & 0xFF;
    this->write_buf[1] = (uint8_t)(f->id & 0xFF);
    this->write_buf[2] = (uint8_t)f->dlc & 0xFF;
    memcpy(&this->write_buf[3], &f->data[0], f->dlc);

    //ppoll(this->fd, 1, 0);
    write(this->fd, &this->write_buf[0], 11);
    tcdrain(this->fd);
}

bool esp32_forwarder::is_port_open() const {
    return this->fd != -1;
}

void esp32_forwarder::close_port() {
    tcdrain(this->fd);
    close(this->fd);
}

esp32_forwarder::esp32_forwarder() {
    this->fd = -1;
}
