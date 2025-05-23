#pragma once
#include "movement_stabilizer_if.h"
#include "drv/pid/pidWrapper.h"

namespace MM
{

class TwoWallStabilizer : public MovementStabilizerIF
{
public:
    TwoWallStabilizer( uint16_t const& dist_frontleft, uint16_t const& dist_frontright );
    int16_t executeControlling() override;
    bool isApplicable() const override;
    void refreshMyTarget() override { }

    void print() const override;
protected:
    // measure points
    uint16_t const& mDistFrontLeftR_mm;
    uint16_t const& mDistFrontRightR_mm;

    PidWrapper myPid;
};

}