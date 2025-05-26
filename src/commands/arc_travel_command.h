#pragma once
#include "motion_command_if.h"
#include "target_speed_calculator.h"
#include "constants.h"
#include <cstdint>
#include "drv/pid/pidWrapper.h"
#include "misc/encoder_value_integrator.h"

namespace MM {

class ArcTravelCommand : public MotionCommandIF
{
public:
    // angle_deg: positive = right turn, negative = left turn
    ArcTravelCommand(float radius_mm, float angle_deg, float speed_mm_per_s, float acc_mm_per_s2, float dec_mm_per_s2,
                    int64_t const& encoderValueLeft, int64_t const& encoderValueRight,
                    int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV);

    void execute() override;
    bool isFinished() const override { return mFinished; }
    CommandResult getResult() override;
    void print() const override;

private:
    int16_t calcVoltageFromSpeed_mV(float setSpeed_mm_per_s);

    // Arc parameters
    float mRadius_mm;
    float mAngle_deg;
    float mSpeed_mm_per_s;
    float mArcLength_mm;
    float mDirection; // +1 for left, -1 for right

    TargetSpeedCalculator myTargetSpeedCalculator;
    EncoderValueIntegrator myEncIntegratorLeft;
    EncoderValueIntegrator myEncIntegratorRight;
    PidWrapper myMovementCtrl{30, 300, 0.1};

    int16_t& mLeftMotorVoltageR_mV;
    int16_t& mRightMotorVoltageR_mV;

    // state flags
    bool mStarted{false};
    bool mFinished{false};

    // time variables (in ms)
    unsigned long mElapsedTime_ms{0};
    unsigned long mStartTime_ms{0};
    unsigned long mTotalTimeOfTravel_ms{0};

    // distance variables
    float mRealCurrentPosition_mm{0}; // average of both wheels
    float mDesiredCurrentPosition_mm{0};
};

} // namespace MM