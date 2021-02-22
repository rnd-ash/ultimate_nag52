#ifndef ART_258H
#define ART_258H

#include <stdint.h>

#ifdef FW_MODE
#include <can_common.h>
#endif

#define ART_258_ID 0x0258

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets ART error code
    void set_ART_ERR(uint8_t value){ raw = (raw & 0xf0ffffffffffffff) | ((uint64_t)value & 0xf) << 56; }
    // Gets ART error code
    uint8_t get_ART_ERR() { return raw >> 56 & 0xf; }

    // Sets ART info lamp
    void set_ART_INFO(bool value){ raw = (raw & 0xefffffffffffffff) | ((uint64_t)value & 0x1) << 60; }
    // Gets ART info lamp
    bool get_ART_INFO() { return raw >> 60 & 0x1; }

    // Sets ART warning tone
    void set_ART_WT(bool value){ raw = (raw & 0xdfffffffffffffff) | ((uint64_t)value & 0x1) << 61; }
    // Gets ART warning tone
    bool get_ART_WT() { return raw >> 61 & 0x1; }

    // Sets Detection of stationary object
    void set_S_OBJ(bool value){ raw = (raw & 0xbfffffffffffffff) | ((uint64_t)value & 0x1) << 62; }
    // Gets Detection of stationary object
    bool get_S_OBJ() { return raw >> 62 & 0x1; }

    // Sets Switch display to ART display
    void set_ART_DSPL_EIN(bool value){ raw = (raw & 0x7fffffffffffffff) | ((uint64_t)value & 0x1) << 63; }
    // Gets Switch display to ART display
    bool get_ART_DSPL_EIN() { return raw >> 63 & 0x1; }

    // Sets set ART speed
    void set_V_ART(uint8_t value){ raw = (raw & 0xff00ffffffffffff) | ((uint64_t)value & 0xff) << 48; }
    // Gets set ART speed
    uint8_t get_V_ART() { return raw >> 48 & 0xff; }

    // Sets Distance relevant object
    void set_ABST_R_OBJ(uint8_t value){ raw = (raw & 0xffff00ffffffffff) | ((uint64_t)value & 0xff) << 40; }
    // Gets Distance relevant object
    uint8_t get_ABST_R_OBJ() { return raw >> 40 & 0xff; }

    // Sets driver's desired distance
    void set_SOLL_ABST(uint8_t value){ raw = (raw & 0xffffff00ffffffff) | ((uint64_t)value & 0xff) << 32; }
    // Gets driver's desired distance
    uint8_t get_SOLL_ABST() { return raw >> 32 & 0xff; }

    // Sets ART-Tempomat switched on
    void set_TM_EIN_ART(bool value){ raw = (raw & 0xfffffffffeffffff) | ((uint64_t)value & 0x1) << 24; }
    // Gets ART-Tempomat switched on
    bool get_TM_EIN_ART() { return raw >> 24 & 0x1; }

    // Sets speed display flashing
    void set_ART_DSPL_BL(bool value){ raw = (raw & 0xfffffffffdffffff) | ((uint64_t)value & 0x1) << 25; }
    // Gets speed display flashing
    bool get_ART_DSPL_BL() { return raw >> 25 & 0x1; }

    // Sets Switch on ART segment display
    void set_ART_SEG_EIN(bool value){ raw = (raw & 0xfffffffffbffffff) | ((uint64_t)value & 0x1) << 26; }
    // Gets Switch on ART segment display
    bool get_ART_SEG_EIN() { return raw >> 26 & 0x1; }

    // Sets Relevant object recognized
    void set_OBJ_ERK(bool value){ raw = (raw & 0xfffffffff7ffffff) | ((uint64_t)value & 0x1) << 27; }
    // Gets Relevant object recognized
    bool get_OBJ_ERK() { return raw >> 27 & 0x1; }

    // Sets Adaptive cruise control switched on
    void set_ART_EIN(bool value){ raw = (raw & 0xffffffffefffffff) | ((uint64_t)value & 0x1) << 28; }
    // Gets Adaptive cruise control switched on
    bool get_ART_EIN() { return raw >> 28 & 0x1; }

    // Sets Indication "---" on the display
    void set_ART_DSPL_LIM(bool value){ raw = (raw & 0xffffffffdfffffff) | ((uint64_t)value & 0x1) << 29; }
    // Gets Indication "---" on the display
    bool get_ART_DSPL_LIM() { return raw >> 29 & 0x1; }

    // Sets Indication "DTR AUS [0]" on the display
    void set_ART_VFBR(bool value){ raw = (raw & 0xffffffffbfffffff) | ((uint64_t)value & 0x1) << 30; }
    // Gets Indication "DTR AUS [0]" on the display
    bool get_ART_VFBR() { return raw >> 30 & 0x1; }

    // Sets "Winter tire limit reached" on the display
    void set_ART_DSPL_PGB(bool value){ raw = (raw & 0xffffffff7fffffff) | ((uint64_t)value & 0x1) << 31; }
    // Gets "Winter tire limit reached" on the display
    bool get_ART_DSPL_PGB() { return raw >> 31 & 0x1; }

    // Sets speed of detected target vehicle
    void set_V_ZIEL(uint8_t value){ raw = (raw & 0xffffffffff00ffff) | ((uint64_t)value & 0xff) << 16; }
    // Gets speed of detected target vehicle
    uint8_t get_V_ZIEL() { return raw >> 16 & 0xff; }

    // Sets Active function
    void set_ASSIST_FKT_AKT(uint8_t value){ raw = (raw & 0xfffffffffffffcff) | ((uint64_t)value & 0x3) << 8; }
    // Gets Active function
    uint8_t get_ASSIST_FKT_AKT() { return raw >> 8 & 0x3; }

    // Sets LED distance assistant flash
    void set_AAS_LED_BL(bool value){ raw = (raw & 0xfffffffffffffbff) | ((uint64_t)value & 0x1) << 10; }
    // Gets LED distance assistant flash
    bool get_AAS_LED_BL() { return raw >> 10 & 0x1; }

    // Sets Object offer distance assistant
    void set_OBJ_AGB(bool value){ raw = (raw & 0xfffffffffffff7ff) | ((uint64_t)value & 0x1) << 11; }
    // Gets Object offer distance assistant
    bool get_OBJ_AGB() { return raw >> 11 & 0x1; }

    // Sets ART distance warning is switched on
    void set_ART_ABW_AKT(bool value){ raw = (raw & 0xffffffffffffefff) | ((uint64_t)value & 0x1) << 12; }
    // Gets ART distance warning is switched on
    bool get_ART_ABW_AKT() { return raw >> 12 & 0x1; }

    // Sets Display of the system availability after system errors
    void set_ART_REAKT(bool value){ raw = (raw & 0xffffffffffffdfff) | ((uint64_t)value & 0x1) << 13; }
    // Gets Display of the system availability after system errors
    bool get_ART_REAKT() { return raw >> 13 & 0x1; }

    // Sets ART is overridden by the driver
    void set_ART_UEBERSP(bool value){ raw = (raw & 0xffffffffffffbfff) | ((uint64_t)value & 0x1) << 14; }
    // Gets ART is overridden by the driver
    bool get_ART_UEBERSP() { return raw >> 14 & 0x1; }

    // Sets Re-trigger the minimum display time
    void set_ART_DSPL_NEU(bool value){ raw = (raw & 0xffffffffffff7fff) | ((uint64_t)value & 0x1) << 15; }
    // Gets Re-trigger the minimum display time
    bool get_ART_DSPL_NEU() { return raw >> 15 & 0x1; }

    // Sets assistance system display request
    void set_ASSIST_ANZ_V2(uint8_t value){ raw = (raw & 0xffffffffffffffe0) | ((uint64_t)value & 0x1f) << 0; }
    // Gets assistance system display request
    uint8_t get_ASSIST_ANZ_V2() { return raw >> 0 & 0x1f; }

    // Sets CAS display request
    void set_CAS_ERR_ANZ_V2(uint8_t value){ raw = (raw & 0xffffffffffffff1f) | ((uint64_t)value & 0x7) << 5; }
    // Gets CAS display request
    uint8_t get_CAS_ERR_ANZ_V2() { return raw >> 5 & 0x7; }

    void import_frame(CAN_FRAME &f) {
        if (f.id == ART_258_ID) {
            for (int i = 0; i < f.length; i++) {
                bytes[7-i] = f.data.bytes[i];
            }
        }
    }

    void export_frame(CAN_FRAME &f) {
        f.id = ART_258_ID;
        f.length = 8;
        f.priority = 4;
        f.rtr = false;
        f.extended = false;
        for (int i = 0; i < 7; i++) {
            f.data.bytes[i] = bytes[7-i];
        }
    }
} ART_258;

#endif
