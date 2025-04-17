#include "rotation_command.h"
#include "Arduino.h" // for millis()
#include <cstdlib>
#include "utils/logging.h"
#include "constants.h"

MM::RotationCommand::RotationCommand(RotationOrientation direction, float angleToRotate_deg, float const& currentOriR, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV):
myTargetSpeedCalculator(static_cast<uint32_t>( (angleToRotate_deg / 360) * 1000 ), 0.5, 0.01, 0.01), // speed: miliRev/ms, acceleration: miliRev/ms^2
myDircetion(direction),
myCurrentOriR_deg(currentOriR),
mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
mRightMotorVoltageR_mV(rightMotorVoltage_mV)
{
    mTotalTimeOfTravel_ms = myTargetSpeedCalculator.getTotalTimeOfTravel_Ms();
    myMovementCtrl.init(1, AUTOMATIC, 0 , 1400); // QUESTION: should the min and the max value be bounded to the current voltage somehow??
}

void MM::RotationCommand::execute()
{
    if( mFinished ) { return; }

    unsigned long now_ms = millis();
    if( !mStarted )
    {
        mStartTime_ms = now_ms;
        mStarted = true;
        myPreviousOrientation_deg = myCurrentOriR_deg;
    }

    // determining times
    unsigned long oldElapsedTime_ms = mElapsedTime_ms;
    mElapsedTime_ms = now_ms - mStartTime_ms;
    unsigned long timeChange_ms = mElapsedTime_ms - oldElapsedTime_ms;

    if( mElapsedTime_ms >= mTotalTimeOfTravel_ms )
    {
        // TODO: maybe the final motor voltage adjustment should be the responsibility of the caller
        mLeftMotorVoltageR_mV = 0;
        mRightMotorVoltageR_mV = 0;
        mFinished = true;
    }
    else
    {
        // Calculating feed forward; theoretical values:
        float outputSpeed_urev_per_ms = myTargetSpeedCalculator.calcCurrentTargetSpeed_UmPerMs( mElapsedTime_ms );
        mDesiredCurrentPosition_urev += outputSpeed_urev_per_ms * timeChange_ms;

        // Real values
        float traveledDistance_deg = std::abs( std::abs( myCurrentOriR_deg ) - std::abs( myPreviousOrientation_deg ) );
        int32_t traveledDistance_urev = static_cast<int32_t>( (traveledDistance_deg / 360) * 1000);
        mRealCurrentPosition_urev += traveledDistance_urev;  
        myPreviousOrientation_deg = myCurrentOriR_deg;

        // PID controlling
        myMovementCtrl.setTarget( static_cast<double>( mDesiredCurrentPosition_urev ) );
        myMovementCtrl.compute( static_cast<double>( mRealCurrentPosition_urev) );

        // Determining output voltage
        int16_t outputVoltage = static_cast<int16_t>(calcVoltageFromSpeed_mV(outputSpeed_urev_per_ms)) + static_cast<int16_t>( myMovementCtrl.getOuput() );

        if( myDircetion == CLOCKWISE )
        {
            mLeftMotorVoltageR_mV = outputVoltage;
            mRightMotorVoltageR_mV = -outputVoltage;
        }
        else
        {
            mLeftMotorVoltageR_mV = -outputVoltage;
            mRightMotorVoltageR_mV = outputVoltage;
        }
    }
}

int16_t MM::RotationCommand::calcVoltageFromSpeed_mV( float setSpeed_um_per_ms )
{
    return static_cast<int16_t>(CONSTS::K_SPEED_FF_REV * setSpeed_um_per_ms + CONSTS::K_BIAS_FF_REV);
}

void MM::RotationCommand::print() const
{
    LOG_INFO("TOT_T: %d ELAPS_T: %d DDIST(urev): %d RDIST(urev): %d ",
        mTotalTimeOfTravel_ms,
        mElapsedTime_ms, 
        static_cast<int> ( mDesiredCurrentPosition_urev),
        static_cast<int> ( mRealCurrentPosition_urev )
      );
}