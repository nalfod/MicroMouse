#pragma once
#include "movement_stabilizer_if.h"
#include "drv/pid/pidWrapper.h"

namespace MM
{

class OrientationStabilizer : public MovementStabilizerIF
{
public:
    OrientationStabilizer( float const& currentOriR );
    int16_t executeControlling() override;
    bool isApplicable() const override;
    void refreshMyTarget() override;

    void print() const override;
private:
    double _shiftOrientationValueRespectedToTarget(float currentOrientation);
    float _adjustAngleToExactDirection( float currentOrientation ); // TODO: USE THE ONE IN CONSTS HEADER WHEN IT IS AVAILABLE!!!
    // measure points
    float const& myCurrentOriR_deg;

    PidWrapper myPid;
    bool refreshIsLocked{false};
};

}