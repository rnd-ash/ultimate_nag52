#include <iostream>
#include "canbus/car_sim.h"
#include "ecus/ms/engine.h"
#include "ecus/bs/abs_esp.h"
#include "ecus/gs/nag52.h"
#include <thread>
#include "v_kombi/virtual_kombi.h"
int main(int argc, char *argv[]) {
    CAR_SIMULATOR sim((char*)"/dev/ttyUSB0");
    sim.init();
    virtual_kombi(&sim).loop();
    return 0;
}
