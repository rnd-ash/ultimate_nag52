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
                Serial.print(x.buffer[i], HEX);
                Serial.print(" ");
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
        Serial.println("Starting diag session!");
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
                    Serial.println("Changing diag server state!");
                    this->respond_ok(sid, &args[0], 1);
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
    } else if (sid == SID_TESTER_PRESENT) {
        if (arg_len == 1) {
            // Respond or not?
            this->last_tp_time = millis();
            if (args[0] == 0x01) { // Response requested
                this->respond_ok(sid, nullptr, 0);
            }
        } else {
            this->respond_err(sid, ERR_GENERAL_REJECT);
        }
    }
    else if (sid == SID_READ_ECU_IDENTIFICATION) {
         if (arg_len == 1) {
             if (args[0] == 0x86) { // DCS ECU identification
                char buf[16];
                //Part number
                buf[0] = 0x03;
                buf[1] = 0x45;
                buf[2] = 0x99;
                buf[3] = 0x99;
                buf[4] = 0x99;

                buf[5] = 0x20;
                buf[6] = 0x21;
                buf[7] = 0x20;
                buf[8] = 0x21;
                buf[9] = 0x08;

                buf[10] = 0x02;
                buf[11] = 0x51;
                buf[12] = 0x00;
                buf[13] = 0x20;
                buf[14] = 0x05;
                buf[15] = 0x21;
                this->respond_ok(sid, args[0], &buf, 16);
             } else if (args[0] == 0x9A || args[0] == 0x9B || args[0] == 0x9F) { // 4.4.4-7
                char buf[35];
                memset(buf, 0x00, 35);
                buf[0] = 1;
                this->respond_ok(sid, args[0], &buf, 35);
             } else if (args[0] == 0x9C || args[0] == 0x9D || args[0] == 0x9E) { // 4.4.4-8
                char buf[6];
                memset(buf, 0x00, 6);
                buf[0] = 1;
                this->respond_ok(sid, args[0], &buf, 6);
             }
         } else {
             this->respond_err(sid, ERR_SUB_FUNC_NOT_SUPPORTED);
         }
    } else {
        Serial.println("Unsupported Service ID");
        this->respond_err(sid, ERR_SERVICE_NOT_SUPPORTED);
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
    ret.buffer_len = data_len+2;
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
    ret.buffer_len = data_len+1;
    ret.buffer[0] = sid + 0x40;
    if (data != 0) {
        memcpy(&ret.buffer[1], data, data_len);
    }
    this->payload_mgr->tx_payload(ret);
}