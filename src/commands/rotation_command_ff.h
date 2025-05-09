#pragma once
#include "motion_command_if.h"
#include "target_speed_calculator.h"
#include <cstdint>
#include "drv/pid/pidWrapper.h"

namespace MM {

// NOTE: This is a legacy commend, use the only PID variant instead, it is more reliable and cleaner!
// This is done by using feed forward, the downfall is that the exit criteria is to measure the elapsed time so it is not the precise
class RotationCommandFF : public MotionCommandIF
{
public:
    RotationCommandFF(float angleToRotate_deg, float const& currentOriR, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV);

    void execute() override;
        
    bool isFinished() const override { return mFinished; }

    // For debug:
    void print() const override;

private:
    enum RotationOrientation
    {
        CLOCKWISE = 1,
        COUNTER_CLOCKWISE = 2
    };

    int16_t calcVoltageFromSpeed_mV( float setSpeed_um_per_ms );

    TargetSpeedCalculator myTargetSpeedCalculator;
    PidWrapper myMovementCtrl{2.5, 1.5, 0.018}; // tuned for 90deg rotation
    float const& myCurrentOriR_deg;
    RotationOrientation myDircetion;

    float myPreviousOrientation_deg{0.0}; // this has the same point as the EncoderValueIntegrator in case of linear travel

    // state flags
    bool mStarted{false};
    bool mFinished{false};

    // time variables (in ms)
    unsigned long mElapsedTime_ms{0};
    unsigned long mStartTime_ms{0};
    unsigned long mTotalTimeOfTravel_ms{0};

    // distance variables (in urev which is milirev!! one rev is 1000 milirev)
    int32_t mRealCurrentPosition_urev{0};
    int32_t mDesiredCurrentPosition_urev{0};

    // controlled units
    int16_t& mLeftMotorVoltageR_mV;
    int16_t& mRightMotorVoltageR_mV;
};

} // namespace MM