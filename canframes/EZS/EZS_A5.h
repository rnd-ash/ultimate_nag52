#ifndef EZS_A5_H
#define EZS_A5_H

#include <stdint.h>

#ifdef FW_MODE
#include <can_common.h>
#endif

#define EZS_A5_ID 0x001F

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets left / right hand drive
    void set_LL_RL(uint8_t value){ raw = (raw & 0xfcffffffffffffff) | ((uint64_t)value & 0x3) << 56; }
    // Gets left / right hand drive
    uint8_t get_LL_RL() { return raw >> 6 & 0x3; }

    // Sets Country-specific SA coding
    void set_LAND(uint8_t value){ raw = (raw & 0x0fffffffffffffff) | ((uint64_t)value & 0xf) << 60; }
    // Gets Country-specific SA coding
    uint8_t get_LAND() { return raw >> 0 & 0xf; }

    // Sets Taxi International
    void set_TAXI_INT(bool value){ raw = (raw & 0xfffeffffffffffff) | ((uint64_t)value & 0x1) << 48; }
    // Gets Taxi International
    bool get_TAXI_INT() { return raw >> 15 & 0x1; }

    // Sets Taxi emergency alarm system
    void set_TAXI_NOTALM(bool value){ raw = (raw & 0xfffdffffffffffff) | ((uint64_t)value & 0x1) << 49; }
    // Gets Taxi emergency alarm system
    bool get_TAXI_NOTALM() { return raw >> 14 & 0x1; }

    // Sets connection for roof signs
    void set_TAXI_DZ(bool value){ raw = (raw & 0xfffbffffffffffff) | ((uint64_t)value & 0x1) << 50; }
    // Gets connection for roof signs
    bool get_TAXI_DZ() { return raw >> 13 & 0x1; }

    // Sets taxi call for help
    void set_TAXI_HIRU(bool value){ raw = (raw & 0xfff7ffffffffffff) | ((uint64_t)value & 0x1) << 51; }
    // Gets taxi call for help
    bool get_TAXI_HIRU() { return raw >> 12 & 0x1; }

    // Sets special vehicle
    void set_SO_FZG(bool value){ raw = (raw & 0xffefffffffffffff) | ((uint64_t)value & 0x1) << 52; }
    // Gets special vehicle
    bool get_SO_FZG() { return raw >> 11 & 0x1; }

    // Sets radio activation taxi
    void set_TAXI_FUNKAUF(bool value){ raw = (raw & 0xffdfffffffffffff) | ((uint64_t)value & 0x1) << 53; }
    // Gets radio activation taxi
    bool get_TAXI_FUNKAUF() { return raw >> 10 & 0x1; }

    // Sets Conversion for the disabled (only via tester)
    void set_BEHI_FZG(bool value){ raw = (raw & 0xffbfffffffffffff) | ((uint64_t)value & 0x1) << 54; }
    // Gets Conversion for the disabled (only via tester)
    bool get_BEHI_FZG() { return raw >> 9 & 0x1; }

    // Sets special protection Guard B4
    void set_GUARD_B4(bool value){ raw = (raw & 0xff7fffffffffffff) | ((uint64_t)value & 0x1) << 55; }
    // Gets special protection Guard B4
    bool get_GUARD_B4() { return raw >> 8 & 0x1; }

    // Sets special protection Guard B6 / 7
    void set_GUARD_B6(bool value){ raw = (raw & 0xfffffeffffffffff) | ((uint64_t)value & 0x1) << 40; }
    // Gets special protection Guard B6 / 7
    bool get_GUARD_B6() { return raw >> 23 & 0x1; }

    // Sets radio data transmission available
    void set_DATENF(bool value){ raw = (raw & 0xfffffdffffffffff) | ((uint64_t)value & 0x1) << 41; }
    // Gets radio data transmission available
    bool get_DATENF() { return raw >> 22 & 0x1; }

    // Sets close fresh air flap when the engine starts
    void set_FL_ZU_MS(bool value){ raw = (raw & 0xfffffbffffffffff) | ((uint64_t)value & 0x1) << 42; }
    // Gets close fresh air flap when the engine starts
    bool get_FL_ZU_MS() { return raw >> 21 & 0x1; }

    // Sets Lock automatic startup FH-rear
    void set_FH_SPERR_HI(bool value){ raw = (raw & 0xfffff7ffffffffff) | ((uint64_t)value & 0x1) << 43; }
    // Gets Lock automatic startup FH-rear
    bool get_FH_SPERR_HI() { return raw >> 20 & 0x1; }

    // Sets Block automatic start-up FH-front
    void set_FH_SPERR_VO(bool value){ raw = (raw & 0xffffefffffffffff) | ((uint64_t)value & 0x1) << 44; }
    // Gets Block automatic start-up FH-front
    bool get_FH_SPERR_VO() { return raw >> 19 & 0x1; }

    // Sets Comfort run mode
    void set_KB_AUTO(bool value){ raw = (raw & 0xffffdfffffffffff) | ((uint64_t)value & 0x1) << 45; }
    // Gets Comfort run mode
    bool get_KB_AUTO() { return raw >> 18 & 0x1; }

    // Sets Comfort circulating air mode
    void set_KB_MAN_KLA(bool value){ raw = (raw & 0xffffbfffffffffff) | ((uint64_t)value & 0x1) << 46; }
    // Gets Comfort circulating air mode
    bool get_KB_MAN_KLA() { return raw >> 17 & 0x1; }

    // Sets Block recirculated air comfort actuation
    void set_KB_SPERR_KLA(bool value){ raw = (raw & 0xffff7fffffffffff) | ((uint64_t)value & 0x1) << 47; }
    // Gets Block recirculated air comfort actuation
    bool get_KB_SPERR_KLA() { return raw >> 16 & 0x1; }

    // Sets vehicle code series
    void set_FCOD_BR(uint8_t value){ raw = (raw & 0xffffffe0ffffffff) | ((uint64_t)value & 0x1f) << 32; }
    // Gets vehicle code series
    uint8_t get_FCOD_BR() { return raw >> 27 & 0x1f; }

    // Sets vehicle code body (203/209)
    void set_FCOD_KAR(uint8_t value){ raw = (raw & 0xffffff1fffffffff) | ((uint64_t)value & 0x7) << 37; }
    // Gets vehicle code body (203/209)
    uint8_t get_FCOD_KAR() { return raw >> 24 & 0x7; }

    // Sets Vehicle code engine
    void set_FCOD_MOT7(uint8_t value){ raw = (raw & 0xffffffff80ffffff) | ((uint64_t)value & 0x7f) << 24; }
    // Gets Vehicle code engine
    uint8_t get_FCOD_MOT7() { return raw >> 33 & 0x7f; }

    // Sets Flat roll warning device available
    void set_PRW_VH(bool value){ raw = (raw & 0xffffffff7fffffff) | ((uint64_t)value & 0x1) << 31; }
    // Gets Flat roll warning device available
    bool get_PRW_VH() { return raw >> 32 & 0x1; }

    // Sets communication platform available
    void set_KP_VH(bool value){ raw = (raw & 0xfffffffffffeffff) | ((uint64_t)value & 0x1) << 16; }
    // Gets communication platform available
    bool get_KP_VH() { return raw >> 47 & 0x1; }

    // Sets driver's seat memory available
    void set_MEMORY_VH(bool value){ raw = (raw & 0xfffffffffffdffff) | ((uint64_t)value & 0x1) << 17; }
    // Gets driver's seat memory available
    bool get_MEMORY_VH() { return raw >> 46 & 0x1; }

    // Sets comfort manual transmission available
    void set_KSG_VH(bool value){ raw = (raw & 0xfffffffffffbffff) | ((uint64_t)value & 0x1) << 18; }
    // Gets comfort manual transmission available
    bool get_KSG_VH() { return raw >> 45 & 0x1; }

    // Sets automatic transmission available
    void set_NAG_VH(bool value){ raw = (raw & 0xfffffffffff7ffff) | ((uint64_t)value & 0x1) << 19; }
    // Gets automatic transmission available
    bool get_NAG_VH() { return raw >> 44 & 0x1; }

    // Sets air conditioning available
    void set_KLA_VH(bool value){ raw = (raw & 0xffffffffffefffff) | ((uint64_t)value & 0x1) << 20; }
    // Gets air conditioning available
    bool get_KLA_VH() { return raw >> 43 & 0x1; }

    // Sets headlight cleaning system available
    void set_SRA_VH(bool value){ raw = (raw & 0xffffffffffdfffff) | ((uint64_t)value & 0x1) << 21; }
    // Gets headlight cleaning system available
    bool get_SRA_VH() { return raw >> 42 & 0x1; }

    // Sets Xenon light available
    void set_XEN_VH(bool value){ raw = (raw & 0xffffffffffbfffff) | ((uint64_t)value & 0x1) << 22; }
    // Gets Xenon light available
    bool get_XEN_VH() { return raw >> 41 & 0x1; }

    // Sets rain sensor available
    void set_RS_VH(bool value){ raw = (raw & 0xffffffffff7fffff) | ((uint64_t)value & 0x1) << 23; }
    // Gets rain sensor available
    bool get_RS_VH() { return raw >> 40 & 0x1; }

    // Sets sound system available
    void set_SOUND_VH(bool value){ raw = (raw & 0xfffffffffffffeff) | ((uint64_t)value & 0x1) << 8; }
    // Gets sound system available
    bool get_SOUND_VH() { return raw >> 55 & 0x1; }

    // Sets level control available
    void set_NIV_VH(bool value){ raw = (raw & 0xfffffffffffffbff) | ((uint64_t)value & 0x1) << 10; }
    // Gets level control available
    bool get_NIV_VH() { return raw >> 53 & 0x1; }

    // Sets Booster fan not available
    void set_BOOSTER_NVH(bool value){ raw = (raw & 0xfffffffffffff7ff) | ((uint64_t)value & 0x1) << 11; }
    // Gets Booster fan not available
    bool get_BOOSTER_NVH() { return raw >> 52 & 0x1; }

    // Sets Footwell flaps in cooling mode, closed. (only G463)
    void set_FUK_SCHL(bool value){ raw = (raw & 0xffffffffffffefff) | ((uint64_t)value & 0x1) << 12; }
    // Gets Footwell flaps in cooling mode, closed. (only G463)
    bool get_FUK_SCHL() { return raw >> 51 & 0x1; }

    // Sets windshield heater. existing
    void set_FSB_HZG_VH(bool value){ raw = (raw & 0xffffffffffffdfff) | ((uint64_t)value & 0x1) << 13; }
    // Gets windshield heater. existing
    bool get_FSB_HZG_VH() { return raw >> 50 & 0x1; }

    // Sets CVT available
    void set_CVT_VH(bool value){ raw = (raw & 0xffffffffffffbfff) | ((uint64_t)value & 0x1) << 14; }
    // Gets CVT available
    bool get_CVT_VH() { return raw >> 49 & 0x1; }

    // Sets ART available
    void set_ART_VH(bool value){ raw = (raw & 0xffffffffffff7fff) | ((uint64_t)value & 0x1) << 15; }
    // Gets ART available
    bool get_ART_VH() { return raw >> 48 & 0x1; }

    // Sets heated windscreen washer system available
    void set_SWB_VH(bool value){ raw = (raw & 0xfffffffffffffffe) | ((uint64_t)value & 0x1) << 0; }
    // Gets heated windscreen washer system available
    bool get_SWB_VH() { return raw >> 63 & 0x1; }

    // Sets Complete replacement light allowed
    void set_ERS_LICHT(bool value){ raw = (raw & 0xfffffffffffffffd) | ((uint64_t)value & 0x1) << 1; }
    // Gets Complete replacement light allowed
    bool get_ERS_LICHT() { return raw >> 62 & 0x1; }

    // Sets Keyless Go available
    void set_KG_VH(bool value){ raw = (raw & 0xfffffffffffffffb) | ((uint64_t)value & 0x1) << 2; }
    // Gets Keyless Go available
    bool get_KG_VH() { return raw >> 61 & 0x1; }

    // Sets interior protection available
    void set_IRS_VH(bool value){ raw = (raw & 0xfffffffffffffff7) | ((uint64_t)value & 0x1) << 3; }
    // Gets interior protection available
    bool get_IRS_VH() { return raw >> 60 & 0x1; }

    // Sets anti-theft alarm system available
    void set_EDW_VH(bool value){ raw = (raw & 0xffffffffffffffef) | ((uint64_t)value & 0x1) << 4; }
    // Gets anti-theft alarm system available
    bool get_EDW_VH() { return raw >> 59 & 0x1; }

    // Sets rear roller blind available
    void set_HR_VH(bool value){ raw = (raw & 0xffffffffffffffdf) | ((uint64_t)value & 0x1) << 5; }
    // Gets rear roller blind available
    bool get_HR_VH() { return raw >> 58 & 0x1; }

    // Sets trailer coupling available
    void set_AHK_VH(bool value){ raw = (raw & 0xffffffffffffffbf) | ((uint64_t)value & 0x1) << 6; }
    // Gets trailer coupling available
    bool get_AHK_VH() { return raw >> 57 & 0x1; }

    // Sets Parktronics system available
    void set_PTS_VH(bool value){ raw = (raw & 0xffffffffffffff7f) | ((uint64_t)value & 0x1) << 7; }
    // Gets Parktronics system available
    bool get_PTS_VH() { return raw >> 56 & 0x1; }

    void import_frame(CAN_FRAME &f) {
        if (f.id == EZS_A5_ID) {
            for (int i = 0; i < f.length; i++) {
                bytes[7-i] = f.data.bytes[i];
            }
        }
    }

    void export_frame(CAN_FRAME &f) {
        f.id = EZS_A5_ID;
        f.length = 8;
        f.priority = 4;
        f.rtr = false;
        f.extended = false;
        for (int i = 0; i < 7; i++) {
            f.data.bytes[i] = bytes[7-i];
        }
    }
} EZS_A5;

#endif EZS_A5_H
