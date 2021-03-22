#include "profiles.h"

GS_FSC abstract_nag_profile::get_display_gear() {
    switch(gs418.get_GZC()) {
        case GS_GZC::N:
            return GS_FSC::N;
        case GS_GZC::D1:
        case GS_GZC::D2:
        case GS_GZC::D3:
        case GS_GZC::D4:
        case GS_GZC::D5:
            return GS_FSC::D;
        case GS_GZC::R:
        case GS_GZC::R2:
            return GS_FSC::R;
        case GS_GZC::P:
            return GS_FSC::P;
        default:
            return GS_FSC::SNV;
    }
    return GS_FSC::SNV;
}


GS_FPC sport_profile::get_current_profile() {
    return GS_FPC::S;
}

void sport_profile::update(nag_iface* iface, shift_api* s) {
    if(ms308.get_NMOT() > 3000) {
        s->upshift(150);
    }
}

PROFILE sport_profile::get_profile_type() {
    return PROFILE::SPORT;
}
