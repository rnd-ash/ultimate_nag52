#include <esp32_can.h>
#include "A0.h"
#include <can_common.h>
#include "canframes/GS/GS_218.h"
#include "canframes/GS/enums.h"
#include "canframes/GS/GS_338.h"
#include "canframes/GS/GS_418.h"

CAN_FRAME f;

GS_218 gs218;
GS_418 gs418;
GS_338 gs338;

// Engine
uint8_t ms_210[8] = {0x00, 0x00, 0x48, 0x40, 0x00, 0x00, 0x80, 0x00};
uint8_t ms_212[8] = {0x03, 0x0C, 0x28, 0xB9, 0x28, 0xB9, 0xA8, 0xB9};
uint8_t ms_2f3[8] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00};
uint8_t ms_308[8] = {0x00, 0x04, 0x73, 0x00, 0x00, 0x68, 0xC3, 0x3A};
uint8_t ms_312[8] = {0x07, 0x68, 0x07, 0x7C, 0x0A, 0x45, 0x07, 0x4A};
uint8_t ms_608[8] = {0x6F, 0x43, 0x06, 0x2D, 0xFA, 0x00, 0x8C, 0x00};
uint8_t ezs_240[8]= {0x00, 0x22, 0x00, 0x00, 0x00, 0x02, 0xFF, 0x00};

// ABS/ESP
uint8_t bs_200[8] = {0x10, 0x44, 0x01, 0xA5, 0x01, 0xAC, 0x01, 0xA7};
uint8_t bs_208[8] = {0x00, 0x20, 0x00, 0x00, 0x01, 0xA9, 0x01, 0xAE};
uint8_t bs_270[8] = {0x6F, 0x96, 0x06, 0xFF, 0xFF, 0x00, 0x00, 0xFF};
uint8_t bs_300[8] = {0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF};
uint8_t bs_328[8] = {0xFF, 0xFF, 0x85, 0x00, 0x00, 0x03, 0x95, 0x0B};

// EZS
uint8_t ezs_a5[8] = {0x02, 0x00, 0x24, 0x06, 0x2D, 0x18, 0x18, 0x1A};

void setup() {
    Serial.begin(115200);
    CAN0.setCANPins(CAN0_RX, CAN0_TX);
    CAN0.enable();
    CAN0.begin(500000);

    gs418.raw = 0x504D7404DD00C000;
    gs418.set_WHST(GearSelectorPos::D);
    gs418.set_GIC(GEAR::G_D1);
    gs418.set_GZC(GEAR::G_D1);
    //gs218.raw = 0x0000DD4923003060;
}

void copy_frame(uint8_t* src, int len) {
    for (int i = 0; i < len; i++) {
        f.data.bytes[i] = src[i];
    }
}

unsigned long last_time = millis();
unsigned long b_time = millis();
unsigned long change_time = millis();

DrivingProgram progs[5] = {
    DrivingProgram::A,
    DrivingProgram::C,
    DrivingProgram::F,
    DrivingProgram::S,
    DrivingProgram::W,
};

FSC steps[16] = {
    FSC::SS_P,
    FSC::SS_R,
    FSC::SS_N,
    FSC::SS_D,
    FSC::SS_D,
    FSC::SS_Speed1,
    FSC::SS_Speed2,
    FSC::SS_Speed3,
    FSC::SS_Speed4,
    FSC::SS_Speed5,
    FSC::SS_Speed6,
    FSC::SS_Speed7,
    FSC::SS_F,
    FSC::SS_F,
    FSC::SS_A,
    FSC::SS_A,
};

int fsc_idx = 0;
int idx = 0;
void loop() {
    if (millis() - change_time >= 333) {
        change_time = millis();
        gs418.set_FSC(steps[fsc_idx]);
        fsc_idx++;
        if (fsc_idx > 16) {
            fsc_idx = 0;
            gs418.set_FPC(progs[idx]);
            idx++;
            if (idx >= 5) {
                idx = 0;
            }
        }
        
    }
    if (millis() - b_time >= 100) {
        b_time = millis();
        // Custom ezs frame
        f.id = 0x001F;
        f.length = 8;
        copy_frame(ezs_a5, 8);
        CAN0.sendFrame(f);
    }

    if (millis() - last_time >= 10) {
        last_time = millis();

        gs218.export_frame(f);
        CAN0.sendFrame(f);

        gs338.export_frame(f);
        CAN0.sendFrame(f);

        gs418.export_frame(f);
        CAN0.sendFrame(f);

        f.length = 8;

        // Engine

        f.id = 0x210;
        copy_frame(ms_210, 8);
        CAN0.sendFrame(f);

        f.id = 0x212;
         copy_frame(ms_212, 8);
        CAN0.sendFrame(f);

        f.id = 0x02f3;
         copy_frame(ms_2f3, 8);
        CAN0.sendFrame(f);

        f.id = 0x0308;
         copy_frame(ms_308, 8);
        CAN0.sendFrame(f);

        f.id = 0x0312;
         copy_frame(ms_312, 8);
        CAN0.sendFrame(f);

        f.id = 0x0608;
         copy_frame(ms_608, 8);
        CAN0.sendFrame(f);


        f.id = 0x0240;
        copy_frame(ezs_240, 8);
        CAN0.sendFrame(f);

        // ABS

        f.id = 0x0200;
        copy_frame(bs_200, 8);
        CAN0.sendFrame(f);

        f.id = 0x0208;
        copy_frame(bs_208, 8);
        CAN0.sendFrame(f);

        f.id = 0x0270;
        copy_frame(bs_270, 8);
        CAN0.sendFrame(f);

        f.id = 0x0300;
        copy_frame(bs_300, 8);
        CAN0.sendFrame(f);

        f.id = 0x0328;
        copy_frame(bs_328, 8);
        CAN0.sendFrame(f);
    }
}