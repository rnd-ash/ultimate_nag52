#ifndef MS_210_H_
#define MS_210_H_

#include <stdint.h>
#include "can_c_enums.h"

#define MS_210_ID 0x0210

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets MS request: "Approach 1st gear"
    void set_ANF1(bool value){ raw = (raw & 0xfeffffffffffffff) | ((uint64_t)value & 0x1) << 56; }
    // Gets MS request: "Approach 1st gear"
    bool get_ANF1() { return raw >> 56 & 0x1; }

    // Sets Switch off KSG creep
    void set_CREEP_AUS(bool value){ raw = (raw & 0xfdffffffffffffff) | ((uint64_t)value & 0x1) << 57; }
    // Gets Switch off KSG creep
    bool get_CREEP_AUS() { return raw >> 57 & 0x1; }

    // Sets switching line shift MS
    void set_SLV_MS(uint8_t value){ raw = (raw & 0x87ffffffffffffff) | ((uint64_t)value & 0xf) << 59; }
    // Gets switching line shift MS
    uint8_t get_SLV_MS() { return raw >> 59 & 0xf; }

    // Sets Air conditioning compressor emergency shutdown
    void set_KOMP_NOTAUS(bool value){ raw = (raw & 0x7fffffffffffffff) | ((uint64_t)value & 0x1) << 63; }
    // Gets Air conditioning compressor emergency shutdown
    bool get_KOMP_NOTAUS() { return raw >> 63 & 0x1; }

    // Sets target gear, lower limit
    void set_GMIN_MS(uint8_t value){ raw = (raw & 0xfff8ffffffffffff) | ((uint64_t)value & 0x7) << 48; }
    // Gets target gear, lower limit
    uint8_t get_GMIN_MS() { return raw >> 48 & 0x7; }

    // Sets target gear, upper limit
    void set_GMAX_MS(uint8_t value){ raw = (raw & 0xffc7ffffffffffff) | ((uint64_t)value & 0x7) << 51; }
    // Gets target gear, upper limit
    uint8_t get_GMAX_MS() { return raw >> 51 & 0x7; }

    // Sets switch off auxiliary heater
    void set_ZH_AUS_MS(bool value){ raw = (raw & 0xffbfffffffffffff) | ((uint64_t)value & 0x1) << 54; }
    // Gets switch off auxiliary heater
    bool get_ZH_AUS_MS() { return raw >> 54 & 0x1; }

    // Sets MS request: "Active downshift"
    void set_AKT_R_MS(bool value){ raw = (raw & 0xff7fffffffffffff) | ((uint64_t)value & 0x1) << 55; }
    // Gets MS request: "Active downshift"
    bool get_AKT_R_MS() { return raw >> 55 & 0x1; }

    // Sets pedal value
    void set_PW(uint8_t value){ raw = (raw & 0xffff00ffffffffff) | ((uint64_t)value & 0xff) << 40; }
    // Gets pedal value
    uint8_t get_PW() { return raw >> 40 & 0xff; }

    // Sets crash signal from engine control
    void set_CRASH_MS(bool value){ raw = (raw & 0xfffffffeffffffff) | ((uint64_t)value & 0x1) << 32; }
    // Gets crash signal from engine control
    bool get_CRASH_MS() { return raw >> 32 & 0x1; }

    // Sets Switch off air conditioning compressor: acceleration
    void set_KOMP_BAUS(bool value){ raw = (raw & 0xfffffffdffffffff) | ((uint64_t)value & 0x1) << 33; }
    // Gets Switch off air conditioning compressor: acceleration
    bool get_KOMP_BAUS() { return raw >> 33 & 0x1; }

    // Sets Motor start / stop system active
    void set_MSS_AKT(bool value){ raw = (raw & 0xfffffffbffffffff) | ((uint64_t)value & 0x1) << 34; }
    // Gets Motor start / stop system active
    bool get_MSS_AKT() { return raw >> 34 & 0x1; }

    // Sets Motor start / stop system control lamp
    void set_MSS_KL(bool value){ raw = (raw & 0xfffffff7ffffffff) | ((uint64_t)value & 0x1) << 35; }
    // Gets Motor start / stop system control lamp
    bool get_MSS_KL() { return raw >> 35 & 0x1; }

    // Sets Motor start / stop system defective
    void set_MSS_DEF(bool value){ raw = (raw & 0xffffffefffffffff) | ((uint64_t)value & 0x1) << 36; }
    // Gets Motor start / stop system defective
    bool get_MSS_DEF() { return raw >> 36 & 0x1; }

    // Sets preheating status
    void set_VGL_ST(bool value){ raw = (raw & 0xffffffdfffffffff) | ((uint64_t)value & 0x1) << 37; }
    // Gets preheating status
    bool get_VGL_ST() { return raw >> 37 & 0x1; }

    // Sets idle is stable
    void set_LL_STBL(bool value){ raw = (raw & 0xffffffbfffffffff) | ((uint64_t)value & 0x1) << 38; }
    // Gets idle is stable
    bool get_LL_STBL() { return raw >> 38 & 0x1; }

    // Sets retrigger the minimum display time
    void set_V_DSPL_NEU(bool value){ raw = (raw & 0xffffff7fffffffff) | ((uint64_t)value & 0x1) << 39; }
    // Gets retrigger the minimum display time
    bool get_V_DSPL_NEU() { return raw >> 39 & 0x1; }

    // Sets warning buzzer on
    void set_V_MAX_SUM(bool value){ raw = (raw & 0xfffffffffeffffff) | ((uint64_t)value & 0x1) << 24; }
    // Gets warning buzzer on
    bool get_V_MAX_SUM() { return raw >> 24 & 0x1; }

    // Sets emergency operation
    void set_NOTL(bool value){ raw = (raw & 0xfffffffffdffffff) | ((uint64_t)value & 0x1) << 25; }
    // Gets emergency operation
    bool get_NOTL() { return raw >> 25 & 0x1; }

    // Sets Kickdown (changeover scenario open!)
    void set_KD_MS(bool value){ raw = (raw & 0xfffffffffbffffff) | ((uint64_t)value & 0x1) << 26; }
    // Gets Kickdown (changeover scenario open!)
    bool get_KD_MS() { return raw >> 26 & 0x1; }

    // Sets speed limit switched on
    void set_V_MAX_EIN(bool value){ raw = (raw & 0xfffffffff7ffffff) | ((uint64_t)value & 0x1) << 27; }
    // Gets speed limit switched on
    bool get_V_MAX_EIN() { return raw >> 27 & 0x1; }

    // Sets Tempomat regulates
    void set_TM_REG(bool value){ raw = (raw & 0xffffffffefffffff) | ((uint64_t)value & 0x1) << 28; }
    // Gets Tempomat regulates
    bool get_TM_REG() { return raw >> 28 & 0x1; }

    // Sets Req. Converter lock-up clutch "slip"
    void set_KUEB_S_A(bool value){ raw = (raw & 0xffffffffdfffffff) | ((uint64_t)value & 0x1) << 29; }
    // Gets Req. Converter lock-up clutch "slip"
    bool get_KUEB_S_A() { return raw >> 29 & 0x1; }

    // Sets idle
    void set_LL(bool value){ raw = (raw & 0xffffffffbfffffff) | ((uint64_t)value & 0x1) << 30; }
    // Gets idle
    bool get_LL() { return raw >> 30 & 0x1; }

    // Sets Pedal encoder error
    void set_PWG_ERR(bool value){ raw = (raw & 0xffffffff7fffffff) | ((uint64_t)value & 0x1) << 31; }
    // Gets Pedal encoder error
    bool get_PWG_ERR() { return raw >> 31 & 0x1; }

    // Sets Speed ​​limiter / cruise control display on
    void set_V_DSPL_EIN(bool value){ raw = (raw & 0xfffffffffffeffff) | ((uint64_t)value & 0x1) << 16; }
    // Gets Speed ​​limiter / cruise control display on
    bool get_V_DSPL_EIN() { return raw >> 16 & 0x1; }

    // Sets display flashes
    void set_V_DSPL_BL(bool value){ raw = (raw & 0xfffffffffffdffff) | ((uint64_t)value & 0x1) << 17; }
    // Gets display flashes
    bool get_V_DSPL_BL() { return raw >> 17 & 0x1; }

    // Sets "Error" indication on the display
    void set_V_DSPL_ERR(bool value){ raw = (raw & 0xfffffffffffbffff) | ((uint64_t)value & 0x1) << 18; }
    // Gets "Error" indication on the display
    bool get_V_DSPL_ERR() { return raw >> 18 & 0x1; }

    // Sets Display "Limit?" on the display
    void set_V_DSPL_LIM(bool value){ raw = (raw & 0xfffffffffff7ffff) | ((uint64_t)value & 0x1) << 19; }
    // Gets Display "Limit?" on the display
    bool get_V_DSPL_LIM() { return raw >> 19 & 0x1; }

    // Sets speed limiter regulates
    void set_V_MAX_REG(bool value){ raw = (raw & 0xffffffffffefffff) | ((uint64_t)value & 0x1) << 20; }
    // Gets speed limiter regulates
    bool get_V_MAX_REG() { return raw >> 20 & 0x1; }

    // Sets Tempomat switched on
    void set_TM_EIN(bool value){ raw = (raw & 0xffffffffffdfffff) | ((uint64_t)value & 0x1) << 21; }
    // Gets Tempomat switched on
    bool get_TM_EIN() { return raw >> 21 & 0x1; }

    // Sets "Winter tire limit reached" on the display
    void set_V_DSPL_PGB(bool value){ raw = (raw & 0xffffffffffbfffff) | ((uint64_t)value & 0x1) << 22; }
    // Gets "Winter tire limit reached" on the display
    bool get_V_DSPL_PGB() { return raw >> 22 & 0x1; }

    // Sets FBS: Start Error
    void set_FBS_SE(bool value){ raw = (raw & 0xffffffffff7fffff) | ((uint64_t)value & 0x1) << 23; }
    // Gets FBS: Start Error
    bool get_FBS_SE() { return raw >> 23 & 0x1; }

    // Sets factor for depreciation. d. Max. Mom. When decreasing A. pressure
    void set_FMMOTMAX(uint8_t value){ raw = (raw & 0xffffffffffff00ff) | ((uint64_t)value & 0xff) << 8; }
    // Gets factor for depreciation. d. Max. Mom. When decreasing A. pressure
    uint8_t get_FMMOTMAX() { return raw >> 8 & 0xff; }

    // Sets Set maximum or cruise control speed
    void set_V_MAX_TM(uint8_t value){ raw = (raw & 0xffffffffffffff00) | ((uint64_t)value & 0xff) << 0; }
    // Gets Set maximum or cruise control speed
    uint8_t get_V_MAX_TM() { return raw >> 0 & 0xff; }

    void import_frame(uint32_t cid, uint8_t* data, uint8_t len) {
        if (cid == MS_210_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = MS_210_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} MS_210;

#endif
