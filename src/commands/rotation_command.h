#pragma once
#include "motion_command_if.h"
#include <cstdint>
#include "drv/pid/pidWrapper.h"

namespace MM {

class RotationCommand : public MotionCommandIF
{
public:
    enum RotationOrientation
    {
        CLOCKWISE = 1,
        COUNTER_CLOCKWISE = 2
    };

    RotationCommand(RotationOrientation direction, float angleToRotate_deg, float const& currentOriR, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV);

    void execute() override;
        
    bool isFinished() const override { return mFinished; }

    // For debug:
    void print() const override;

private:
    PidWrapper myMovementCtrl{15, 7, 3.5};

    // goal
    RotationOrientation const myDircetion;
    float const myTargetMagnitude_deg;

    // state flags
    bool mStarted{false};
    bool mFinished{false};

    // Ori values
    float myPreviousOrientation_deg{0.0};
    float myTraveledDistance_deg{0.0};
    float const& myCurrentOriR_deg;

    // controlled units
    int16_t& mLeftMotorVoltageR_mV;
    int16_t& mRightMotorVoltageR_mV;

};

} // namespace MM