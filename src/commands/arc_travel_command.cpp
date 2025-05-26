#include "arc_travel_command.h"
#include "Arduino.h"
#include "utils/logging.h"
#include <cmath>

////////////////////
/// ArcTravelCommand
////////////////////

MM::ArcTravelCommand::ArcTravelCommand(float radius_mm, float angle_deg, float speed_mm_per_s, float acc_mm_per_s2, float dec_mm_per_s2,
                                    int64_t const& encoderValueLeft, int64_t const& encoderValueRight,
                                    int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV)
    : mRadius_mm(radius_mm),
      mAngle_deg(angle_deg),
      mSpeed_mm_per_s(speed_mm_per_s),
      mArcLength_mm( std::abs(angle_deg) * M_PI / 180.0f * radius_mm ),
      mDirection(angle_deg >= 0 ? 1.0f : -1.0f),
      myTargetSpeedCalculator(mArcLength_mm, speed_mm_per_s, acc_mm_per_s2, dec_mm_per_s2),
      myEncIntegratorLeft(encoderValueLeft),
      myEncIntegratorRight(encoderValueRight),
      mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
      mRightMotorVoltageR_mV(rightMotorVoltage_mV)
{
    mTotalTimeOfTravel_ms = myTargetSpeedCalculator.getTotalTimeOfTravel_Ms();
    myMovementCtrl.init(1, AUTOMATIC, -1000, 1000);
}

void MM::ArcTravelCommand::execute()
{
    if (mFinished) { return; }

    unsigned long now_ms = millis();
    if (!mStarted)
    {
        mStartTime_ms = now_ms;
        mStarted = true;
        myEncIntegratorLeft.startIntegration();
        myEncIntegratorRight.startIntegration();
    }

    unsigned long oldElapsedTime_ms = mElapsedTime_ms;
    mElapsedTime_ms = now_ms - mStartTime_ms;
    unsigned long timeChange_ms = mElapsedTime_ms - oldElapsedTime_ms;

    if (mElapsedTime_ms >= mTotalTimeOfTravel_ms)
    {
        mLeftMotorVoltageR_mV = 0;
        mRightMotorVoltageR_mV = 0;
        mFinished = true;
    }
    else
    {
        // Target speed along the arc
        float outputSpeed_mm_per_s = myTargetSpeedCalculator.calcCurrentTargetSpeed_mmPerS(mElapsedTime_ms);
        mDesiredCurrentPosition_mm += static_cast<float>(outputSpeed_mm_per_s * timeChange_ms) / 1000.0f;

        // Real distance traveled (average of both wheels)
        float traveledLeft_mm = myEncIntegratorLeft.getTraveledDistanceSinceLastInvoke_mm();
        float traveledRight_mm = myEncIntegratorRight.getTraveledDistanceSinceLastInvoke_mm();
        float traveledDistance_mm = (traveledLeft_mm + traveledRight_mm) / 2.0f;
        mRealCurrentPosition_mm += traveledDistance_mm;

        // PID controlling
        myMovementCtrl.setTarget(static_cast<double>(mDesiredCurrentPosition_mm));
        myMovementCtrl.compute(static_cast<double>(mRealCurrentPosition_mm));

        // Calculate wheel speeds for arc
        // V_left = V * (R + W/2) / R, V_right = V * (R - W/2) / R
        constexpr float WHEEL_BASE_MM = CONSTS::WHEEL_BASE_MM; // define in constants.h
        float v = outputSpeed_mm_per_s + static_cast<float>(myMovementCtrl.getOuput());
        float v_left = v * (mRadius_mm + mDirection * WHEEL_BASE_MM / 2.0f) / mRadius_mm;
        float v_right = v * (mRadius_mm - mDirection * WHEEL_BASE_MM / 2.0f) / mRadius_mm;

        int16_t voltageLeft = calcVoltageFromSpeed_mV(v_left);
        int16_t voltageRight = calcVoltageFromSpeed_mV(v_right);

        mLeftMotorVoltageR_mV = voltageLeft;
        mRightMotorVoltageR_mV = voltageRight;
    }
}

int16_t MM::ArcTravelCommand::calcVoltageFromSpeed_mV(float setSpeed_mm_per_s)
{
    int16_t outputVoltageAbs_mV = static_cast<int16_t>(CONSTS::K_SPEED_FF * std::abs(setSpeed_mm_per_s) + CONSTS::K_BIAS_FF);

    if (setSpeed_mm_per_s > 0.0)
    {
        return outputVoltageAbs_mV;
    }
    else if (setSpeed_mm_per_s < 0.0)
    {
        return -outputVoltageAbs_mV;
    }
    else
    {
        return 0;
    }
}

void MM::ArcTravelCommand::print() const
{
    LOG_INFO("ARC_TRAV_CMD: TOT_T: %d ELAPS_T: %d DDIST: %d RDIST: %d PID_OUT: %d ANGLE: %d RADIUS: %d\n",
        mTotalTimeOfTravel_ms,
        mElapsedTime_ms,
        static_cast<int>(mDesiredCurrentPosition_mm),
        static_cast<int>(mRealCurrentPosition_mm),
        static_cast<int>(myMovementCtrl.getOuput()),
        static_cast<int>(mAngle_deg),
        static_cast<int>(mRadius_mm)
    );
}

MM::CommandResult MM::ArcTravelCommand::getResult()
{
    // Distance and angle actually traveled
    float avgDistance = mRealCurrentPosition_mm;
    float angle = mAngle_deg; // Could be refined using encoders if needed
    // TODO: HANDLE THIS CASE IN CELL POSITION AS WELL SOMEHOW!!!!!
    return CommandResult(avgDistance, angle);
}