//
// Created by ashcon on 2/20/21.
//

#include "can_sim.h"


std::string fmt_frame(CAN_FRAME *f) {
    char buf[150] = {0x00};
    int pos = sprintf(buf, "0x%04X - [", f->id);
    for (int i = 0; i < f->length; i++) {
        pos += sprintf(buf+pos, "%02X ", f->data.bytes[i]);
    }
    buf[pos-1] = ']';
    return std::string(buf);
}
