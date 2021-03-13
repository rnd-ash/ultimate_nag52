//
// Created by ashcon on 2/20/21.
//

#include "abs_esp.h"
#include "can_frames.h"

short output_shaft_rpm = 0;

void abs_esp::setup() {
    bs200.raw = 0x104401AD01AD01A7;
    bs208.raw = 0x0020000001A901AE;
    bs270.raw = 0x6F9606FFFF0000FF;
    bs300.raw = 0x0808000000007FFF;
    bs328.raw = 0xFFFF85000003950B;

    // Front wheel rpm
    bs200.set_DVL(0);
    bs200.set_DVR(0);

    // Rear wheel rpm
    bs208.set_DHL(0);
    bs208.set_DHR(0);
}

void abs_esp::simulate_tick() {
    short wheel_rpm = (short)((float)output_shaft_rpm / 2.87 * 2);
    bs200.set_DVL(wheel_rpm);
    bs200.set_DVR(wheel_rpm);
}