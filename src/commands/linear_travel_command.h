#pragma once
#include <cstdint>
#include "drv/pid/pidWrapper.h"

namespace MM {

constexpr float K_SPEED_FF = 1.5405;
constexpr float K_BIAS_FF = 285.19;
constexpr int ENCODER_RESOULTION = 288;
constexpr float WHEEL_DIAMETER = 43.8; // in mm

class TargetSpeedCalculator
{
    public:
        TargetSpeedCalculator(float dist, float speed, float acc, float dec);
        float calcCurrentTargetSpeed(float elapsedTime);
        float getTotalTimeOfTravel() { return mAccelerationTime + mUniformTravelTime + mDecelerationTime; }
    private:
        float calcVelocityInAcc(float accElapsedTime);
        float calcVelocityInDec(float decElapsedTime);

        // inputs
        float mDistance; // mm
        float mSetSpeed; // mm/s
        float mAcceleration; // mm/s^2
        float mDeceleration; // mm/s^2
        
        // segments of the movement
        float mAccelerationTime;
        float mDecelerationTime;
        float mUniformTravelTime;

};

class EncoderValueIntegrator
{
    public:
        EncoderValueIntegrator(int const& encoderResultVar): currentEncoderValue(encoderResultVar) { }
        float getTraveledDistanceSinceLastInvoke()
        {
            float traveledDistance = static_cast<float>( (currentEncoderValue - previousEncoderValue) ) / ENCODER_RESOULTION * WHEEL_DIAMETER * 3.1415;
            previousEncoderValue = currentEncoderValue;
            return traveledDistance; // in mm
        }

    private:
        int const& currentEncoderValue;
        int previousEncoderValue = 0;
};

class LinearTravelCommand
{
    public:
        LinearTravelCommand(float dist, float speed, float acc, float dec, int const& encoderValue1, int const& encoderValue2, int16_t& leftMotorVoltage, int16_t& rightMotorVoltage);
        
        void execute();
        
        bool isFinished() { return mFinished; }
    
    private:
        float calcVoltageFromSpeed( float setSpeed );

        TargetSpeedCalculator myTargetSpeedCalculator;
        EncoderValueIntegrator myEncIntegrator1;
        EncoderValueIntegrator myEncIntegrator2;
        PidWrapper myMovementCtrl{0.5, 0.5, 0.5};

        // state flags
        bool mStarted{false};
        bool mFinished{false};

        // time variables
        float mElapsedTime{0.0};
        float mStartTime{0.0};
        float mTotalTimeOfTravel{0.0};

        // distance variables
        float mRealCurrentPosition{0.0}; // will be read from the encoders
        float mDesiredCurrentPosition{0.0};

        // controlled units
        int16_t& mLeftMotorVoltage;
        int16_t& mRightMotorVoltage;
};

} // namespace MM