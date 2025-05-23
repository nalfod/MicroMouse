#pragma once
#include "two_wall_stabilizer.h"
#include "drv/pid/pidWrapper.h"

namespace MM
{

class OneWallStabilizer : public TwoWallStabilizer
{
public:
    OneWallStabilizer( uint16_t const& dist_frontleft, uint16_t const& dist_frontright );
    int16_t executeControlling() override;
    bool isApplicable() const override;

    void print() const override;
};

}