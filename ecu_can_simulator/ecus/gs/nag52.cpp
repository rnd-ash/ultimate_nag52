//
// Created by ashcon on 2/20/21.
//

#include "nag52.h"

void nag52::setup() {
    gs418.raw = 0x504D7404DD00C000;
    gs218.raw = 0x0000DD4923003060;
    gs338.raw = 0x0000000000000000;
}

void nag52::simulate_tick() {
    gs418.set_ALL_WHEEL(false);
    gs418.set_CVT(false);
    gs418.set_T_GET(80+40); // 80C
    gs218.set_GSP_OK(true);

#ifdef W5A330
    gs418.set_MECH(GearVariant::NAG2_KLEIN2);
#else
    gs418.set_MECH(GearVariant::NAG2_GROSS2);
#endif
}
