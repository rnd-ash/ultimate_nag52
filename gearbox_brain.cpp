
#include "gearbox_brain.h"


void Gearbox::startup(nag_iface* iface) {
    this->iface = iface;

    // On startup we should be in 'S' mode
    this->sports = sport_profile();
    this->current_profile = &sports;

    // Set default CAN Data about NAG
    gs418.set_MECH(iface->gearbox_type); // NAG Type (W580 or W330)
    gs338.set_NAB(0xFFFF); // Always for auto boxes
    gs418.set_ALL_WHEEL(false);
    gs418.set_CVT(false); // Not CVT :)
    gs418.set_ESV_BRE(false); // TODO analyse what this does
    gs418.set_FPC(this->current_profile->get_current_profile());
    gs418.set_FRONT(false); // Always for 722.6
    gs418.set_FSC(this->current_profile->get_display_gear());
    // Gears are unknown at startup!
    gs418.set_GIC(GEAR::G_SNV);
    gs418.set_GZC(GEAR::G_SNV);

    gs418.set_KD(false); // Always - EGS52 and up doesn't monitor kickdown
    gs418.set_SWITCH(false); // TODO analyse what this does
}

void Gearbox::select_next_profile() {

    switch(this->current_profile->get_profile_type()) {
        case PROFILE::SPORT:
            this->current_profile = &this->sports;
            break;
        case PROFILE::COMFORT:
            break;
        case PROFILE::WINTER:
            break;
        case PROFILE::AGILITY:
            break;
        case PROFILE::MANUAL:
            break;
    }
}