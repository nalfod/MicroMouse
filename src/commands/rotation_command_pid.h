#pragma once
#include "motion_command_if.h"
#include <cstdint>
#include "drv/pid/pidWrapper.h"
#include "maze/location_controller.h"

namespace MM {

class RotationCommandPid : public MotionCommandIF
{
public:
    RotationCommandPid(float angleToRotate_deg, float const& currentOriR, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV);

    void execute() override;
    CommandResult getResult() override;
        
    bool isFinished() const override { return mFinished; }

    // For debug:
    void print() const override;

private:
    float shiftOrientationValue(float orientationValue);

    PidWrapper myMovementCtrl{10, 2, 0.08}; // tuned for 90 deg, but works fine for 180 deg too!

    // goal
    float const myTargetMagnitude_deg; // +: clockwise, -: counterclockwise

    // state flags
    bool mStarted{false};
    bool mFinished{false};

    // Ori values
    float myTargetOrientation_deg{0.0};
    float const& myCurrentOriR_deg;

    // controlled units
    int16_t& mLeftMotorVoltageR_mV;
    int16_t& mRightMotorVoltageR_mV;

};

} // namespace MM