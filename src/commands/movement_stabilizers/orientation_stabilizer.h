#pragma once
#include "movement_stabilizer_if.h"
#include "drv/pid/pidWrapper.h"

namespace MM
{

class OrientationStabilizer : public MovementStabilizerIF
{
public:
    OrientationStabilizer( float const& currentOriR, bool stayOnGrid = false );
    int16_t executeControlling() override;
    bool isApplicable() const override;
    void refreshMyTarget() override;

    void print() const override;
private:
    double _shiftOrientationValueRespectedToTarget(float currentOrientation);
    // measure points
    float const& myCurrentOriR_deg;

    PidWrapper myPid;
    bool refreshIsLocked{false};
    bool stayOnGrid{false};
};

}