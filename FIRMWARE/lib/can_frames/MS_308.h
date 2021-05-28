#ifndef MS_308_H_
#define MS_308_H_

#include <stdint.h>
#include "can_c_enums.h"

#define MS_308_ID 0x0308

typedef union {
    uint8_t bytes[8];
    uint64_t raw;

    // Sets cylinder deactivation conditions met
    void set_ZASBED(bool value){ raw = (raw & 0xfeffffffffffffff) | ((uint64_t)value & 0x1) << 56; }
    // Gets cylinder deactivation conditions met
    bool get_ZASBED() { return raw >> 56 & 0x1; }

    // Sets water in fuel control lamp (only CR2-USA)
    void set_WKS_KL(bool value){ raw = (raw & 0xfdffffffffffffff) | ((uint64_t)value & 0x1) << 57; }
    // Gets water in fuel control lamp (only CR2-USA)
    bool get_WKS_KL() { return raw >> 57 & 0x1; }

    // Sets fuel filter clogged control lamp (only CR2-USA)
    void set_KSF_KL(bool value){ raw = (raw & 0xfbffffffffffffff) | ((uint64_t)value & 0x1) << 58; }
    // Gets fuel filter clogged control lamp (only CR2-USA)
    bool get_KSF_KL() { return raw >> 58 & 0x1; }

    // Sets fuel cutoff full
    void set_SASV(bool value){ raw = (raw & 0xf7ffffffffffffff) | ((uint64_t)value & 0x1) << 59; }
    // Gets fuel cutoff full
    bool get_SASV() { return raw >> 59 & 0x1; }

    // Sets partial thrust cut-off
    void set_SAST(bool value){ raw = (raw & 0xefffffffffffffff) | ((uint64_t)value & 0x1) << 60; }
    // Gets partial thrust cut-off
    bool get_SAST() { return raw >> 60 & 0x1; }

    // Sets speed limitation function active
    void set_N_MAX_BG(bool value){ raw = (raw & 0xdfffffffffffffff) | ((uint64_t)value & 0x1) << 61; }
    // Gets speed limitation function active
    bool get_N_MAX_BG() { return raw >> 61 & 0x1; }

    // Sets Req. Torque converter lock-up clutch "open"
    void set_KUEB_O_A(bool value){ raw = (raw & 0xbfffffffffffffff) | ((uint64_t)value & 0x1) << 62; }
    // Gets Req. Torque converter lock-up clutch "open"
    bool get_KUEB_O_A() { return raw >> 62 & 0x1; }

    // Sets clutch pressed
    void set_KPL(bool value){ raw = (raw & 0x7fffffffffffffff) | ((uint64_t)value & 0x1) << 63; }
    // Gets clutch pressed
    bool get_KPL() { return raw >> 63 & 0x1; }

    // Sets engine speed
    void set_NMOT(short value){ raw = (raw & 0xff0000ffffffffff) | ((uint64_t)value & 0xffff) << 40; }
    // Gets engine speed
    short get_NMOT() { return raw >> 40 & 0xffff; }

    // Sets Tank cap open control lamp
    void set_TANK_KL(bool value){ raw = (raw & 0xfffffffeffffffff) | ((uint64_t)value & 0x1) << 32; }
    // Gets Tank cap open control lamp
    bool get_TANK_KL() { return raw >> 32 & 0x1; }

    // Sets Diagnostic control lamp (OBD II)
    void set_DIAG_KL(bool value){ raw = (raw & 0xfffffffdffffffff) | ((uint64_t)value & 0x1) << 33; }
    // Gets Diagnostic control lamp (OBD II)
    bool get_DIAG_KL() { return raw >> 33 & 0x1; }

    // Sets Oil level / oil pressure control lamp
    void set_OEL_KL(bool value){ raw = (raw & 0xfffffffbffffffff) | ((uint64_t)value & 0x1) << 34; }
    // Gets Oil level / oil pressure control lamp
    bool get_OEL_KL() { return raw >> 34 & 0x1; }

    // Sets Pre-glow control lamp
    void set_VGL_KL(bool value){ raw = (raw & 0xfffffff7ffffffff) | ((uint64_t)value & 0x1) << 35; }
    // Gets Pre-glow control lamp
    bool get_VGL_KL() { return raw >> 35 & 0x1; }

    // Sets air filter dirty control lamp (only diesel)
    void set_LUFI_KL(bool value){ raw = (raw & 0xffffffefffffffff) | ((uint64_t)value & 0x1) << 36; }
    // Gets air filter dirty control lamp (only diesel)
    bool get_LUFI_KL() { return raw >> 36 & 0x1; }

    // Sets ethanol operation detected
    void set_EOH(bool value){ raw = (raw & 0xffffffdfffffffff) | ((uint64_t)value & 0x1) << 37; }
    // Gets ethanol operation detected
    bool get_EOH() { return raw >> 37 & 0x1; }

    // Sets Warning message for the eco power steering pump
    void set_ELHP_WARN(bool value){ raw = (raw & 0xffffffbfffffffff) | ((uint64_t)value & 0x1) << 38; }
    // Gets Warning message for the eco power steering pump
    bool get_ELHP_WARN() { return raw >> 38 & 0x1; }

    // Sets cooling water temperature too high
    void set_TEMP_KL(bool value){ raw = (raw & 0xfffffffffeffffff) | ((uint64_t)value & 0x1) << 24; }
    // Gets cooling water temperature too high
    bool get_TEMP_KL() { return raw >> 24 & 0x1; }

    // Sets speed limitation for display active (0 for CR)
    void set_DBAA(bool value){ raw = (raw & 0xfffffffffdffffff) | ((uint64_t)value & 0x1) << 25; }
    // Gets speed limitation for display active (0 for CR)
    bool get_DBAA() { return raw >> 25 & 0x1; }

    // Sets Motor fan defective control lamp
    void set_LUEFT_MOT_KL(bool value){ raw = (raw & 0xfffffffffbffffff) | ((uint64_t)value & 0x1) << 26; }
    // Gets Motor fan defective control lamp
    bool get_LUEFT_MOT_KL() { return raw >> 26 & 0x1; }

    // Sets starter is running
    void set_ANL_LFT(bool value){ raw = (raw & 0xfffffffff7ffffff) | ((uint64_t)value & 0x1) << 27; }
    // Gets starter is running
    bool get_ANL_LFT() { return raw >> 27 & 0x1; }

    // Sets ADR defective control lamp (only commercial vehicles)
    void set_ADR_DEF_KL(bool value){ raw = (raw & 0xffffffffefffffff) | ((uint64_t)value & 0x1) << 28; }
    // Gets ADR defective control lamp (only commercial vehicles)
    bool get_ADR_DEF_KL() { return raw >> 28 & 0x1; }

    // Sets ADR control lamp (commercial vehicles only)
    void set_ADR_KL(bool value){ raw = (raw & 0xffffffffdfffffff) | ((uint64_t)value & 0x1) << 29; }
    // Gets ADR control lamp (commercial vehicles only)
    bool get_ADR_KL() { return raw >> 29 & 0x1; }

    // Sets cylinder deactivation
    void set_ZAS(bool value){ raw = (raw & 0xffffffffbfffffff) | ((uint64_t)value & 0x1) << 30; }
    // Gets cylinder deactivation
    bool get_ZAS() { return raw >> 30 & 0x1; }

    // Sets engine oil temperature too high (overheating)
    void set_UEHITZ(bool value){ raw = (raw & 0xffffffff7fffffff) | ((uint64_t)value & 0x1) << 31; }
    // Gets engine oil temperature too high (overheating)
    bool get_UEHITZ() { return raw >> 31 & 0x1; }

    // Sets oil temperature
    void set_T_OEL(uint8_t value){ raw = (raw & 0xffffffffff00ffff) | ((uint64_t)value & 0xff) << 16; }
    // Gets oil temperature
    uint8_t get_T_OEL() { return raw >> 16 & 0xff; }

    // Sets oil level
    void set_OEL_FS(uint8_t value){ raw = (raw & 0xffffffffffff00ff) | ((uint64_t)value & 0xff) << 8; }
    // Gets oil level
    uint8_t get_OEL_FS() { return raw >> 8 & 0xff; }

    // Sets oil quality
    void set_OEL_QUAL(uint8_t value){ raw = (raw & 0xffffffffffffff00) | ((uint64_t)value & 0xff) << 0; }
    // Gets oil quality
    uint8_t get_OEL_QUAL() { return raw >> 0 & 0xff; }

    void import_frame(uint32_t cid, const uint8_t* data, uint8_t len) {
        if (cid == MS_308_ID) {
            for (int i = 0; i < len; i++) {
                bytes[7-i] = data[i];
            }
        }
    }

    void export_frame(uint32_t* cid, uint8_t* data, uint8_t* len) {
        *cid = MS_308_ID;
        *len = 8;
        for (int i = 0; i < *len; i++) {
            data[i] = bytes[7-i];
        }
    }
} MS_308;

#endif
