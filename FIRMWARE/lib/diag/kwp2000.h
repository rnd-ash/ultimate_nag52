#ifndef KWP2000_H_
#define KWP2000_H_

#include <dtcs.h>

/**
 * KWP 2000 diagnostic server implementation
 * (Emulating stock EGS unless stated otherwise)
 */

#define VENDOR_ID 593 // Used for ECU identification by Daimler software (Simens diagVersion 51)

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



#endif