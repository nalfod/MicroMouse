#pragma once
#include "motion_command_if.h"
#include "constants.h"
#include <cstdint>
#include "drv/pid/pidWrapper.h"

namespace MM {

class TargetSpeedCalculator
{
public:
    TargetSpeedCalculator(uint32_t dist_um, uint32_t speed_um_per_ms, uint32_t acc_um_per_ms2, uint32_t dec_um_per_ms2);
    uint32_t calcCurrentTargetSpeed_UmPerMs(unsigned long  elapsedTime_ms);
    unsigned long getTotalTimeOfTravel_Ms() { return mAccelerationTime_ms + mUniformTravelTime_ms + mDecelerationTime_ms; }
private:
    uint32_t getSpeedInAcc_UmPerMs(unsigned long accElapsedTime_ms);
    uint32_t getSpeedInDec_UmPerMs(unsigned long decElapsedTime_ms);

    // inputs
    uint32_t mDistance_um; // um
    uint32_t mSetSpeed_um_per_ms; // um/ms    note: 1 mm/s == 1 um/ms
    uint32_t mAcceleration_um_per_ms2; // um/ms^2    note: 1 mm/s^2 = 0.001 mm/s^2
    uint32_t mDeceleration_um_per_ms2; // um/ms^2
    
    // segments of the movement
    unsigned long mAccelerationTime_ms;
    unsigned long mDecelerationTime_ms;
    unsigned long mUniformTravelTime_ms;

};










class EncoderValueIntegrator
{
public:
    EncoderValueIntegrator(int64_t const& encoderResultVarR): currentEncoderValueR(encoderResultVarR) { }

    void startIntegration() { previousEncoderValue = currentEncoderValueR; } // this little error costed me ~4 hours of debugging :)

    int32_t getTraveledDistanceSinceLastInvoke_Um()
    {
        int32_t traveledDistance = static_cast<int32_t>( static_cast<float>( (currentEncoderValueR - previousEncoderValue) ) / CONSTS::ENCODER_RESOULTION * CONSTS::WHEEL_DIAMETER_UM * 3.1415 );
        previousEncoderValue = currentEncoderValueR;
        return traveledDistance; // in um
    }

private:
    int64_t const& currentEncoderValueR;
    int64_t previousEncoderValue;
};









class LinearTravelCommand : public MotionCommandIF
{
public:
    LinearTravelCommand(uint32_t dist_um, uint32_t speed_um_per_ms, uint32_t acc_um_per_ms2, uint32_t dec_um_per_ms2, 
                        int64_t const& encoderValue1, int64_t const& encoderValue2, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV);
    
    void execute() override;
    
    bool isFinished() const override { return mFinished; }

    // For debug:
    void print() const override;
    int32_t getRealCurrentPosition_um() const { return mRealCurrentPosition_um; }
    int32_t getDesiredCurrentPosition_um() const { return mDesiredCurrentPosition_um; }
    unsigned long getStartTime_ms() const { return mStartTime_ms; }
    unsigned long getElapsedTime_ms() const { return mElapsedTime_ms; }
    unsigned long getTotalTime_ms() const { return mTotalTimeOfTravel_ms; }
    int32_t mCurrentSpeed_UmPerMs{0};

private:
    int16_t calcVoltageFromSpeed_mV( int16_t setSpeed_um_per_ms );

    TargetSpeedCalculator myTargetSpeedCalculator;
    EncoderValueIntegrator myEncIntegrator1;
    EncoderValueIntegrator myEncIntegrator2;
    PidWrapper myMovementCtrl{0.01, 0, 0};
    // QUESTION: How should this be fine-tuned? Should we change it durint runtime?

    // state flags
    bool mStarted{false};
    bool mFinished{false};

    // time variables (in ms)
    unsigned long mElapsedTime_ms{0};
    unsigned long mStartTime_ms{0};
    unsigned long mTotalTimeOfTravel_ms{0};

    // distance variables (in um!!)
    int32_t mRealCurrentPosition_um{0}; // will be read from the encoders
    int32_t mDesiredCurrentPosition_um{0};

    // controlled units
    int16_t& mLeftMotorVoltageR_mV;
    int16_t& mRightMotorVoltageR_mV;
};

} // namespace MM