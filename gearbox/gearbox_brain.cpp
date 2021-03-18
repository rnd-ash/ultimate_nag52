
#include "gearbox_brain.h"
#include "helper.h"

void Gearbox::startup(nag_iface* iface) {
    this->iface = iface;
    this->shifter = new shift_api(iface);
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
    gs418.set_GIC(GS_GIC::SNV);
    gs418.set_GZC(GS_GZC::SNV);
    gs418.set_FSC(GS_FSC::SNV);

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

void Gearbox::loop() {
    this->shifter->update();
    this->current_profile->update(this->iface);
    gs418.set_FSC(this->current_profile->get_display_gear());
    //LOG_MSG("%s\n", gs418.get_fsc());


    // WHST always follows EWM position!
    GS_WHST whst;
    switch(ewm230.get_WHC()) {
        case EWM_WHC::D:
        case EWM_WHC::PLUS:
        case EWM_WHC::MINUS:
            whst = GS_WHST::D;
            break;
        case EWM_WHC::N:
        case EWM_WHC::N_D:
            whst = GS_WHST::N;
            break;
        case EWM_WHC::R:
        case EWM_WHC::R_N:
            whst = GS_WHST::R;
            break;
        case EWM_WHC::P:
        case EWM_WHC::P_R:
            whst = GS_WHST::P;
            break;
        case EWM_WHC::SNV:
            whst = GS_WHST::SNV;
            break;
    }
    gs418.set_WHST(whst);
}