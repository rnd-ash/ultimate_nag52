#include "profiles.h"

DrivingProgram sport_profile::get_current_profile() {
    return DrivingProgram::S;
}

void sport_profile::update(nag_iface* iface) {
}

FSC sport_profile::get_display_gear() {
    return FSC::SS_Blank;
}

PROFILE sport_profile::get_profile_type() {
    return PROFILE::SPORT;
}