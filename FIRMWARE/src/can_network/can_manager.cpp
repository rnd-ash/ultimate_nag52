#include "can_manager.h"
#include <Arduino.h>
#include <FreeRTOS_TEENSY4.h>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> canc;

IsoTp_Manager* kwp2000_iso_tp_server;


void set_mailbox_interrupt(FLEXCAN_MAILBOX mb, int can_id, _MB_ptr handler) {
    canc.setMBFilter(mb, can_id);
    canc.enableMBInterrupt(mb);
    canc.onReceive(mb, handler);
}

void interrupt_mb_ms_308(const CAN_message_t &msg) {
    ms308.import_frame(msg.id, msg.buf, msg.len);
}

void interrupt_mb_diag_rq(const CAN_message_t &msg) {
    kwp2000_iso_tp_server->interrupt_incoming_frame(msg);
}

void init_can() {
    kwp2000_iso_tp_server = new IsoTp_Manager(20, 8, 0x07E1, 0x07E9);
    canc.begin();
    canc.setBaudRate(500000);
    canc.setMaxMB(NUM_RX_MB + NUM_TX_MB);
    for (int i = 0; i < NUM_RX_MB; i++) {
        canc.setMB((const FLEXCAN_MAILBOX)i, RX, STD);
    }
    for (int i = NUM_RX_MB; i < NUM_RX_MB+NUM_TX_MB; i++) {
        canc.setMB((const FLEXCAN_MAILBOX)i, TX, STD);
    }

    canc.setMBFilter(REJECT_ALL);
    canc.enableMBInterrupts();
    set_mailbox_interrupt(MB_MS_308, MS_308_ID, interrupt_mb_ms_308);
    set_mailbox_interrupt(MB_DIAG_RQ, 0x07E1, interrupt_mb_diag_rq);
    xTaskCreate(IsoTp_Manager::create_task, "ISOTP server (Diag)", 4096, kwp2000_iso_tp_server, 20, NULL);
}


void IsoTp_Manager::create_task(void* params) {
    IsoTp_Manager* who = (IsoTp_Manager*)params;
    who->task();
}

IsoTp_Manager::IsoTp_Manager(uint8_t st_min, uint8_t bs, uint16_t rx_id, uint16_t tx_id) {
    this->st_min = st_min;
    this->bs = bs;
    this->tx_id = tx_id;
    this->rx_id = rx_id;
}

void IsoTp_Manager::task() {
    Serial.println("Task created ISOTP");
    while(1) {
        vTaskDelay(20);
    }
}

bool IsoTp_Manager::read_payload(IsoTpPayload &p) {
    if(this->has_rx_payload) {
        memcpy(&p, &this->rx, sizeof(IsoTpPayload));
        this->has_rx_payload = false;
        return true;
    }
    return false;
}

void IsoTp_Manager::tx_payload(IsoTpPayload tx) {
    CAN_message_t tx_frame;
    tx_frame.id = this->tx_id;
    tx_frame.len = 8;

    if (tx.buffer_len <= 7) {
        // Can Tx in 1 frame!
        tx_frame.buf[0] = tx.buffer_len;
        memcpy(&tx_frame.buf[1], tx.buffer, tx.buffer_len);
        canc.write(tx_frame);
    } else {
        memcpy(&this->tx, &tx, sizeof(IsoTpPayload));
        this->clear_to_send = false;
        this->send_flow_control = true;
        tx_frame.buf[0] = 0x10;
        tx_frame.buf[1] = tx.buffer_len;
        memcpy(&tx_frame.buf[2], tx.buffer, 6);
        this->tx._buf_pos = 6;
        canc.write(tx_frame);
    }
}

void IsoTp_Manager::interrupt_incoming_frame(const CAN_message_t &msg) {
    if (msg.id != this->rx_id || msg.len != 8){return; /* Discard - EGS52 always uses padded ISO-TP */}
    uint8_t pci = msg.buf[0] & 0xF0;
    switch (pci) {
        case 0x00:
            // 1 frame!
            this->rx.buffer_len = msg.buf[0];
            memcpy(&this->rx.buffer[0], &msg.buf[1], msg.buf[0]);
            this->has_rx_payload = true;
            break;
        case 0x10:
            this->send_flow_control = true;
            this->has_rx_payload = false;
            this->rx.buffer_len = msg.buf[1];
            memcpy(&this->rx.buffer[0], &msg.buf[2], 6);
            this->rx._buf_pos = 6;
            break;
        case 0x20:
            {
                if (!is_receiving){break;}
                uint8_t max_cpy = min(7, this->rx.buffer_len - this->rx._buf_pos);
                memcpy(&this->rx.buffer[this->rx._buf_pos], &msg.buf[1], max_cpy);
                this->rx._buf_pos += max_cpy;
                if (this->rx._buf_pos >= this->rx.buffer_len) {
                    this->has_rx_payload = true;
                    this->send_flow_control = false;
                    this->is_receiving = false;
                }
            }
        case 0x30:
            {
                if (msg.buf[0] == 0x30) {
                    this->clear_to_send = true;
                }
            }
        default:
            break;
    }
}