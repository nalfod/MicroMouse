#pragma once
#include "motion_command_if.h"
#include "target_speed_calculator.h"
#include "constants.h"
#include <cstdint>
#include "drv/pid/pidWrapper.h"
#include "misc/encoder_value_integrator.h"

namespace MM {

class LinearTravelCommand : public MotionCommandIF
{
public:
    LinearTravelCommand(float dist_mm, float speed_mm_per_s, float acc_mm_per_s2, float dec_mm_per_s2, 
                        int64_t const& encoderValue1, int64_t const& encoderValue2, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV);

    LinearTravelCommand(float dist_um, float speed_um_per_ms, float acc_um_per_ms2, float dec_um_per_ms2, 
                        int64_t const& encoderValue1, int64_t const& encoderValue2, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV, 
                        double Kp, double Ki, double Kd);
    
    void execute() override;
    CommandResult getResult() override;
    
    bool isFinished() const override { return mFinished; }

    // For debug:
    void print() const override;

private:
    int16_t calcVoltageFromSpeed_mV( float setSpeed_um_per_ms );

    TargetSpeedCalculator myTargetSpeedCalculator;
    EncoderValueIntegrator myEncIntegrator1;
    EncoderValueIntegrator myEncIntegrator2;
    PidWrapper myMovementCtrl{30, 300, 0.1};
    // QUESTION: How should this be fine-tuned? Should we change it durint runtime?

    // state flags
    bool mStarted{false};
    bool mFinished{false};

    // time variables (in ms)
    unsigned long mElapsedTime_ms{0};
    unsigned long mStartTime_ms{0};
    unsigned long mTotalTimeOfTravel_ms{0};

    // distance variables (in um!!)
    float mRealCurrentPosition_mm{0}; // will be read from the encoders
    float mDesiredCurrentPosition_mm{0};

    // controlled units
    int16_t& mLeftMotorVoltageR_mV;
    int16_t& mRightMotorVoltageR_mV;

    // Direction changer
    int16_t const direction;
};

} // namespace MM