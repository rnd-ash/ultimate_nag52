#include "profiles.h"

GS_FPC sport_profile::get_current_profile() {
    return GS_FPC::S;
}

void sport_profile::update(nag_iface* iface) {
}

GS_FSC sport_profile::get_display_gear() {
    return GS_FSC::BLANK;
}

PROFILE sport_profile::get_profile_type() {
    return PROFILE::SPORT;
}