//
// Created by ashcon on 2/20/21.
//

#include "abs_esp.h"


void abs_esp::setup() {
    bs200.raw = 0x104401AD01AD01A7;
    bs208.raw = 0x0020000001A901AE;
    bs270.raw = 0x6F9606FFFF0000FF;
    bs300.raw = 0x0808000000007FFF;
    bs328.raw = 0xFFFF85000003950B;

    bs200.set_DVL(2500);
    bs200.set_DVR(2500);
}

void abs_esp::simulate_tick() {

}