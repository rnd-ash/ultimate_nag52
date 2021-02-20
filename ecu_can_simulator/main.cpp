#include <iostream>
#include "canbus/can_sim.h"
#include "canbus/esp32_forwarder.h"
#include "../firmware/canframes/MS/MS_308.h"
#include "../firmware/canframes/MS/MS_608.h"
#include <thread>
int main() {
    std::cout << "Hello, World!" << std::endl;

    MS_608 ms608;
    MS_308 ms308;
    ms308.set_NMOT(4000); // Actual RPM
    ms308.set_T_OEL(120); // Temp + 40
    ms608.set_T_MOT(80+40); // Temp + 40

    CAN_FRAME res;

    esp32_forwarder x = esp32_forwarder((char*)"/dev/ttyUSB0");
    if (x.is_port_open()) {
        printf("Sending data\n");
        for (int i = 0; i < 100; i++) {
            ms308.export_frame(res);
            x.send_frame(&res);
            ms608.export_frame(res);
            x.send_frame(&res);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    if (x.is_port_open()) {
        x.close_port();
    }
    return 0;
}
