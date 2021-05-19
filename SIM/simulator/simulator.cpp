#include <iostream>
#include "canbus/car_sim.h"
#include "v_kombi/virtual_kombi.h"

int main() {
    CAR_SIMULATOR sim((char*)"/dev/ttyUSB0");
    //sim.init((char*)"/home/ashcon/Desktop/canlogs/canlogs_winnersh-london.log");
    sim.init();
    virtual_kombi(&sim).loop();
    return 0;
}


