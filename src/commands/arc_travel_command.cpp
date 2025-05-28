#include "arc_travel_command.h"
#include "Arduino.h"
#include "utils/logging.h"
#include <cmath>

////////////////////
/// ArcTravelCommand
////////////////////

MM::ArcTravelCommand::ArcTravelCommand(float radius_mm, float angle_deg, float speed_mm_per_s, float acc_mm_per_s2, float dec_mm_per_s2,
                                    int64_t const& encoderValueLeft, int64_t const& encoderValueRight,
                                    int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV):
mRadius_mm(radius_mm),
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
    myMovementCtrlLeft.init(1, AUTOMATIC, -1000, 1000);
    myMovementCtrlRight.init(1, AUTOMATIC, -1000, 1000);
}

MM::ArcTravelCommand::ArcTravelCommand(float radius_mm, float angle_deg, float speed_mm_per_s, float acc_mm_per_s2, float dec_mm_per_s2,
                                       float start_speed_mm_per_s, float end_speed_mm_per_s,                                
                                       int64_t const& encoderValueLeft, int64_t const& encoderValueRight,
                                       int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV):
mRadius_mm(radius_mm),
mAngle_deg(angle_deg),
mSpeed_mm_per_s(speed_mm_per_s),
mArcLength_mm( std::abs(angle_deg) * M_PI / 180.0f * radius_mm ),
mDirection(angle_deg >= 0 ? 1.0f : -1.0f),
myTargetSpeedCalculator(mArcLength_mm, speed_mm_per_s, acc_mm_per_s2, dec_mm_per_s2, start_speed_mm_per_s, end_speed_mm_per_s),
myEncIntegratorLeft(encoderValueLeft),
myEncIntegratorRight(encoderValueRight),
mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
mRightMotorVoltageR_mV(rightMotorVoltage_mV)
{
    mTotalTimeOfTravel_ms = myTargetSpeedCalculator.getTotalTimeOfTravel_Ms();
    myMovementCtrlLeft.init(1, AUTOMATIC, -1000, 1000);
    myMovementCtrlRight.init(1, AUTOMATIC, -1000, 1000);
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
        mFinished = true;
    }
    else
    {
        // Target speed along the arc
        // V_left = V * (R + W/2) / R, V_right = V * (R - W/2) / R
        float outputSpeedCenter_mm_per_s = myTargetSpeedCalculator.calcCurrentTargetSpeed_mmPerS(mElapsedTime_ms);
        float outputSpeedLeft_mm_per_s = outputSpeedCenter_mm_per_s * (mRadius_mm + mDirection * CONSTS::WHEEL_BASE_MM / 2.0f) / mRadius_mm;
        float outputSpeedRight_mm_per_s = outputSpeedCenter_mm_per_s * (mRadius_mm - mDirection * CONSTS::WHEEL_BASE_MM / 2.0f) / mRadius_mm;
        mDesiredCurrentPositionLeft_mm += static_cast<float>(outputSpeedLeft_mm_per_s * timeChange_ms) / 1000.0f;
        mDesiredCurrentPositionRight_mm += static_cast<float>(outputSpeedRight_mm_per_s * timeChange_ms) / 1000.0f;

        // Real distance traveled (average of both wheels)
        float traveledLeft_mm = myEncIntegratorLeft.getTraveledDistanceSinceLastInvoke_mm();
        float traveledRight_mm = myEncIntegratorRight.getTraveledDistanceSinceLastInvoke_mm();
        float traveledDistance_mm = (traveledLeft_mm + traveledRight_mm) / 2.0f;
        mRealCurrentPositionLeft_mm += traveledLeft_mm;
        mRealCurrentPositionRight_mm += traveledRight_mm;

        // PID controlling
        myMovementCtrlLeft.setTarget(static_cast<double>(mDesiredCurrentPositionLeft_mm));
        myMovementCtrlLeft.compute(static_cast<double>(mRealCurrentPositionLeft_mm));
        myMovementCtrlRight.setTarget(static_cast<double>(mDesiredCurrentPositionRight_mm));
        myMovementCtrlRight.compute(static_cast<double>(mRealCurrentPositionRight_mm));

        // Calculate wheel speeds for arc
        
        float v_left = outputSpeedLeft_mm_per_s + static_cast<float>(myMovementCtrlLeft.getOuput());
        float v_right = outputSpeedRight_mm_per_s + static_cast<float>(myMovementCtrlRight.getOuput());

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
    /*
    LOG_INFO("ARC_TRAV_CMD: TOT_T: %d ELAPS_T: %d DDIST: %d RDIST: %d PID_OUT: %d ANGLE: %d RADIUS: %d\n",
        mTotalTimeOfTravel_ms,
        mElapsedTime_ms,
        static_cast<int>(mDesiredCurrentPosition_mm),
        static_cast<int>(mRealCurrentPosition_mm),
        static_cast<int>(myMovementCtrl.getOuput()),
        static_cast<int>(mAngle_deg),
        static_cast<int>(mRadius_mm)
    );*/
}

MM::CommandResult MM::ArcTravelCommand::getResult()
{
    // Distance and angle actually traveled
    auto traveledDistances_mm = _calculateMyResult();
    float angle_deg = mAngle_deg;   

    return CommandResult(traveledDistances_mm.second, traveledDistances_mm.first, angle_deg);
}

std::pair<float, float> MM::ArcTravelCommand::_calculateMyResult()
{
    float x = 0.0;
    float y = 0.0;
    float R_mm = 90.0;
    
    float absAngle_deg = std::abs(mAngle_deg);
    
    float absAngle_rad = absAngle_deg * ( 3.141592654 / 180.0 );
    
    if( 0.0 < absAngle_deg && absAngle_deg <= 90.0 )
    {
        x += mRadius_mm * (1 - std::cos(absAngle_rad));
        y += mRadius_mm * std::sin(absAngle_rad);
    }
    else if( 90.0 < absAngle_deg && absAngle_deg <= 180.0 )
    {
        x += mRadius_mm + mRadius_mm * std::cos(3.141592654 - absAngle_rad);
        y += mRadius_mm * std::sin(absAngle_rad);        
    }
    else if( 180.0 < absAngle_deg && absAngle_deg <= 270.0 )
    {
        x += mRadius_mm + mRadius_mm * std::cos(absAngle_rad - 3.141592654);
        y += mRadius_mm * std::sin(absAngle_rad); 
    }
    
    if( mAngle_deg < 0.0 )
    {
        x *= -1;
    }
    
    return std::pair<float, float>(x, y);
}