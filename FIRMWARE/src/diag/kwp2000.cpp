#include "kwp2000.h"
#include <string.h>

KwpBuffer KWPDiagServer::respond_err(uint8_t sid, uint8_t err) {
    KwpBuffer ret;
    ret.len = 3;
    ret.data[0] = 0x7F;
    ret.data[1] = sid;
    ret.data[2] = err;
    return ret;
}

KwpBuffer KWPDiagServer::respond_ok(uint8_t sid, uint8_t lid, void* data, uint8_t data_len) {
    // Check we won't overflow
    if ((uint16_t)(data_len) + 2 > 0xFF) {
        return respond_err(sid, ERR_BUFFER_OVERFLOW);
    }
    KwpBuffer ret;
    ret.len = (uint8_t)(data_len+2);
    ret.data[0] = sid + 0x40;
    ret.data[1] = lid;
    memcpy(&ret.data[2], data, data_len);
    return ret;
}

KwpBuffer KWPDiagServer::respond_ok(uint8_t sid, void* data, uint8_t data_len) {
    // Check we won't overflow
    if ((uint16_t)(data_len) + 1 > 0xFF) {
        return respond_err(sid, ERR_BUFFER_OVERFLOW);
    }
    KwpBuffer ret;
    ret.len = (uint8_t)(data_len+1);
    ret.data[0] = sid + 0x40;
    memcpy(&ret.data[1], data, data_len);
    return ret;
}