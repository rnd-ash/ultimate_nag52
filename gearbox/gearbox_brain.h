#ifndef GEARBOX_BRAIN_H_
#define GEARBOX_BRAIN_H_


#ifdef SIM_MODE
#include "can_frames.h" // For cmake (sim)
#include "shift_api.h"
#else
#include "../can_frames/can_frames.h" // For arduino
#endif

#include "profiles.h"

#define SPORTS_ENABLE  // Comment to disable sports mode
#define COMFORT_ENABLE // Comment to disable comfort mode
#define WINTER_ENABLE  // Comment to disable winter mode
#define AGILITY_ENABLE // Comment to disable Agility mode
#define MANUAL_ENABLE  // Comment to disable Manual mode

class Gearbox {
public:
    // Called on firmware initialization
    void startup(nag_iface* iface);
    // Called every 10ms 
    void loop();
private:
    void select_next_profile();
private:
    
//#ifdef SPORTS_ENABLE
sport_profile sports;

#endif
#ifdef COMFORT_ENABLE
#endif
#ifdef WINTER_ENABLE
#endif
#ifdef AGILITY_ENABLE
#endif
#ifdef MANUAL_ENABLE

#if !defined(SPORTS_ENABLE) && !defined(COMFORT_ENABLE) && !defined(WINTER_ENABLE) && !defined(AGILITY_ENABLE) && !defined(MANUAL_ENABLE)
    #error "NO PROFILES ENABLED. Please enable at least 1 profile above!"
#endif

// TODO Add fail profile to NAG
//#endif
    abstract_nag_profile* current_profile; // Whats in use now

    nag_iface* iface; // Interface to talk to the gearbox!
    shift_api* shifter; // Shift API
};

#endif