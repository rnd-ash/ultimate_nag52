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
    virtual GS_FPC get_current_profile() = 0;
    virtual void update(nag_iface* iface) = 0; // Call this every 10ms
    // Returns speed step to show on IC
    virtual GS_FSC get_display_gear() = 0;
    virtual PROFILE get_profile_type() = 0;
};

class sport_profile : public abstract_nag_profile {
public:
    GS_FPC get_current_profile() override;
    void update(nag_iface* iface) override;
    GS_FSC get_display_gear() override;
    PROFILE get_profile_type() override;
private:
    GS_FSC speedstep = GS_FSC::BLANK;
};

#endif