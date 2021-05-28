#ifndef KWP2000_H_
#define KWP2000_H_

#include <dtcs.h>
#include "kwp_structs.h"
#include <FreeRTOS_TEENSY4.h>
#include "can_network/can_manager.h"

/**
 * KWP 2000 diagnostic server implementation
 * (Emulating stock EGS unless stated otherwise)
 */

#define DAIMLER_VENDOR_ID 593 // Used for ECU identification by Daimler software (Simens diagVersion 51)

#define P1_MIN 2500 // Milliseconds until ECU goes back into default diag session state

// Service IDs
#define SID_START_DIAG_SESSION 0x10
#define SID_ECU_RESET 0x11
#define SID_CLEAR_DIAG_INFO 0x14
#define SID_READ_STATUS_DTC 0x17
#define SID_READ_ECU_IDENTIFICATION 0x1A
#define SID_READ_DATA_BY_LID 0x21
#define SID_READ_DATA_BY_ID 0x22
#define SID_READ_MEM_BY_ADDR 0x23
#define SID_SECURITY_ACCESS 0x27
#define SID_DISABLE_NORM_MSG_TX 0x28
#define SID_ENABLE_NORM_MSG_TX 0x29
#define SID_DYNAMICALLY_DEFINE_LID 0x2C
#define SID_WRITE_DATA_BY_ID 0x2E
#define SID_IOCTL_BY_LID 0x30
#define SID_START_ROUTINE_BY_LID 0x31
#define SID_STOP_ROUTINE_BY_LID 0x32
#define SID_REQUEST_ROUTINE_RESULTS_BY_LID 0x33
#define SID_REQUEST_DOWNLOAD 0x34
#define SID_REQUEST_UPLOAD 0x35
#define SID_TRANSFER_DATA 0x36
#define SID_TRANSFER_EXIT 0x37
#define SID_WRITE_DATA_BY_LID 0x3B
#define SID_WRITE_MEMORY_BY_ADDR 0x3D
#define SID_TESTER_PRESENT 0x3E
#define SID_CONTROL_DTC_SETTINGS 0x85
#define SID_RESPOND_ON_EVENT 0x86

// Additional SIDs TBA (For ultimate-nag52 only)
// TBA


// -- Diagnostic session types --
#define SESSION_DEFAULT 0x81 // Default session on boot
#define SESSION_FLASH 0x85 // Flash session
#define SESSION_STANDBY 0x89 // Standby session
#define SESSION_PASSIVE 0x90 // Passive session
#define SESSION_EXTENDED 0x92 // Extended diag session


// -- Error codes --
#define ERR_GENERAL_REJECT 0x10
#define ERR_SERVICE_NOT_SUPPORTED 0x11
#define ERR_SUB_FUNC_NOT_SUPPORTED 0x12 // Or Invalid format
#define ERR_BUSY 0x21
#define ERR_CONDITIONS_NOT_CORRECT 0x22
#define ERR_ROUTINE_NOT_COMPLETE 0x23
#define ERR_REQUEST_OUT_OF_RANGE 0x31
#define ERR_SECURITY_ACCESS_DENIED 0x33
#define ERR_INVALID_KEY 0x35
#define ERR_EXCEEDED_NUM_ATTEMPTS 0x36
#define ERR_TIME_DELAY_NOT_EXPIRED 0x37
#define ERR_DOWNLOAD_NOT_ACCEPTED 0x40
#define ERR_UPLOAD_NOT_ACCEPTED 0x50
#define ERR_TRANSFER_SUSPENDED 0x71
#define ERR_RESPONSE_PENDING 0x78
#define ERR_SERVICE_NOT_SUPPORTED_IN_ACTIVE_SESSION 0x80
#define ERR_DECOMPRESSION_FAILED 0x9A
#define ERR_DECRYPTION_FAILED 0x9B
#define ERR_ECU_NOT_RESPONDING 0xA0
#define ERR_ECU_ADDR_UNKNOWN 0xA1
// This error is Ultimate-Nag52 only and is designed for debugging. If a buffer overflows somewhere in the KWP2000 stack,
// this error is returned
#define ERR_BUFFER_OVERFLOW 0xE0

/**
 * Represnets a buffer to Tx back to the diagnostic tester.
 * 
 * EGS52 has a maximum buffer size of 256, so we can just allocate directly in this struct
 * as that is nothing compared to Teensy 4's 1024KB of memory! (On stock EGS52, that is 1/16th of its entire memory!)
 */
struct KwpBuffer {
    uint8_t data[256];
    uint16_t len;
};

class KWPDiagServer {
public:
    KWPDiagServer(IsoTp_Manager* tx_mgr);
    static void create_task(void* params);
    void run_task();
private:

    void process_payload(uint8_t sid, uint8_t* args, uint8_t arg_len);

    /**
     * Generates an Error response given the Service ID and Error code
     */
    void respond_err(uint8_t sid, uint8_t err);

    /**
     * Generates an OK Response given a Service ID and Local ID.
     * Data is copied to the Tx buffer, and must be data_len bytes
     */
    void respond_ok(uint8_t sid, uint8_t lid, void* data, uint8_t data_len);

    /**
     * Generates an OK Response given a Service ID.
     * Data is copied to the Tx buffer, and must be data_len bytes
     */
    void respond_ok(uint8_t, void* data, uint8_t data_len);

    uint8_t server_state = SESSION_DEFAULT;
    unsigned long last_tp_time = 0;

    IsoTp_Manager* payload_mgr;

};


#endif