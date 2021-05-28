#include "kwp2000.h"
#include <string.h>

KWPDiagServer::KWPDiagServer(IsoTp_Manager* tx_mgr) {
    this->payload_mgr = tx_mgr;
    xTaskCreate(KWPDiagServer::create_task, "KWP", 4096, this, 20, NULL);
}


void KWPDiagServer::create_task(void* params) {
    KWPDiagServer* who = (KWPDiagServer*) params;
    who->run_task();
}

void KWPDiagServer::run_task() {
    Serial.println("KWP launch");
    IsoTpPayload x;
    while(1) {
        if (millis() - this->last_tp_time > 2500 && this->server_state != SESSION_DEFAULT) {
            this->server_state = SESSION_DEFAULT;
            Serial.println("Timeout for TP, returning to default state");
        }
        if (this->payload_mgr->read_payload(x)) {
            Serial.print("Incoming diag message: [");
            for(int i = 0; i < x.buffer_len; i++) {
                Serial.print(x.buffer[i]);
            }
            Serial.println("]");
            this->process_payload(x.buffer[0], &x.buffer[1], x.buffer_len-1);
        } else {
            vTaskDelay(10);
        }

    }
}

void KWPDiagServer::process_payload(uint8_t sid, uint8_t* args, uint8_t arg_len) {
    if (sid == SID_START_DIAG_SESSION) {
        Serial.print("Starting diag session!");
        if (arg_len == 1) {
            // Set server state
            switch (args[0]) {
                case SESSION_DEFAULT:
                case SESSION_EXTENDED:
                case SESSION_FLASH:
                case SESSION_PASSIVE:
                case SESSION_STANDBY:
                    this->last_tp_time = millis();
                    this->server_state = args[0];
                    Serial.print("Changing diag server state!");
                    this->respond_ok(sid, nullptr, 0);
                    break;
                default:
                    Serial.print("Invalid LID for START_DIAG_SESSION 0x");
                    Serial.println(args[0]);
                    this->respond_err(sid, ERR_SUB_FUNC_NOT_SUPPORTED);
                    break;
            }
        } else {
            Serial.println("Invalid number of args for START_DIAG_SESSION");
            this->respond_err(sid, ERR_SUB_FUNC_NOT_SUPPORTED);
        }
    }
}

void KWPDiagServer::respond_err(uint8_t sid, uint8_t err) {
    IsoTpPayload ret;
    ret.buffer_len = 3;
    ret.buffer[0] = 0x7F;
    ret.buffer[1] = sid;
    ret.buffer[2] = err;
    this->payload_mgr->tx_payload(ret);
}

void KWPDiagServer::respond_ok(uint8_t sid, uint8_t lid, void* data, uint8_t data_len) {
    // Check we won't overflow
    if ((uint16_t)(data_len) + 2 > 0xFF) {
        return respond_err(sid, ERR_BUFFER_OVERFLOW);
    }
    IsoTpPayload ret;
    ret.buffer_len = (uint8_t)(data_len+2);
    ret.buffer[0] = sid + 0x40;
    ret.buffer[1] = lid;
    if (data != 0) {
        memcpy(&ret.buffer[2], data, data_len);
    }
    this->payload_mgr->tx_payload(ret);
}

void KWPDiagServer::respond_ok(uint8_t sid, void* data, uint8_t data_len) {
    // Check we won't overflow
    if ((uint16_t)(data_len) + 1 > 0xFF) {
        return respond_err(sid, ERR_BUFFER_OVERFLOW);
    }
    IsoTpPayload ret;
    ret.buffer_len = (uint8_t)(data_len+1);
    ret.buffer[0] = sid + 0x40;
    if (data != 0) {
        memcpy(&ret.buffer[1], data, data_len);
    }
    this->payload_mgr->tx_payload(ret);
}