#ifndef NAG_PROFILE_H_
#define NAG_PROFILE_H_

#include "nag_iface.h"

enum class PROFILE {
    SPORT,
    COMFORT,
    WINTER,
    AGILITY,
    MANUAL
};

class abstract_nag_profile {
public:
    // Gets the current status bit (Drive profile indicator)
    virtual DrivingProgram get_current_profile() = 0;
    virtual void update(nag_iface* iface) = 0; // Call this every 10ms
    // Returns speed step to show on IC
    virtual FSC get_display_gear() = 0;
    virtual PROFILE get_profile_type() = 0;
};

class sport_profile : public abstract_nag_profile {
public:
    DrivingProgram get_current_profile() override;
    void update(nag_iface* iface) override;
    FSC get_display_gear() override;
    PROFILE get_profile_type() override;
private:
    FSC speedstep = FSC::SS_Blank;
};

/*
class comfort_profile: public abstract_nag_profile {
public:
    DrivingProgram* get_current_profile();
    void update(nag_iface* iface);
    FSC get_display_gear();
};

class winter_profile: public abstract_nag_profile {
public:
    DrivingProgram* get_current_profile();
    void update(nag_iface* iface);
    FSC get_display_gear();
};

class agility_profile: public abstract_nag_profile {
public:
    DrivingProgram* get_current_profile();
    void update(nag_iface* iface);
    FSC get_display_gear();
};

class manual_profile: public abstract_nag_profile {
public:
    DrivingProgram* get_current_profile();
    void update(nag_iface* iface);
    FSC get_display_gear();
};

// Sadness, gearbox is dead :(
class fail_profile: public abstract_nag_profile {
public:
    DrivingProgram* get_current_profile();
    void update(nag_iface* iface);
    FSC get_display_gear();
};
*/
#endif