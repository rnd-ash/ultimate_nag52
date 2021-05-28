#ifndef CAN_MGR_H_
#define CAN_MGR_H_

#include <FlexCAN_T4.h>
#include <can_frames.h>

#define NUM_TX_MB 8
#define NUM_RX_MB 20

#define MB_ART250H MB0
#define MB_ART_258 MB1
#define MB_BS_200 MB2
#define MB_BS_208 MB3
#define MB_BS_270 MB4
#define MB_BS_300 MB5
#define MB_BS_328 MB6
#define MB_EWM_230 MB7
#define MB_LRW_236 MB8
#define MB_MS_2F3 MB9
#define MB_MS_210 MB10
#define MB_MS_212 MB11
#define MB_MS_268 MB12
#define MB_MS_308 MB13
#define MB_MS_312 MB14
#define MB_MS_608 MB15
#define MB_PSM_3B4 MB16
#define MB_PSM_3B8 MB17
#define MB_SBW_232 MB18
#define MB_DIAG_RQ MB19

void init_can();

struct IsoTpPayload {
    uint8_t buffer[256];
    uint8_t buffer_len;
    uint8_t _buf_pos;
};

class IsoTp_Manager {
public:
    static void create_task(void* params);
    IsoTp_Manager(uint8_t st_min, uint8_t bs, uint16_t rx_id, uint16_t tx_id);
    void interrupt_incoming_frame(const CAN_message_t &msg);
    void tx_payload(IsoTpPayload tx);
    bool read_payload(IsoTpPayload &p);
    void task();
private:
    bool has_rx_payload = false;
    bool has_tx_payload = false;
    bool clear_to_send = false;
    bool send_flow_control = false;
    bool is_receiving = false;
    uint16_t rx_id;
    uint16_t tx_id;
    uint8_t st_min;
    uint8_t bs;

    IsoTpPayload tx;
    IsoTpPayload rx;
};

extern IsoTp_Manager* kwp2000_iso_tp_server;

#endif