#include "rotation_command_pid.h"
#include <cstdlib>
#include "utils/logging.h"
#include "constants.h"

// DONT USE IT TO TURN MORE THAN 180 (it doesnt make any sense anyway)
MM::RotationCommandPid::RotationCommandPid(float angleToRotate_deg, float const& currentOriR, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV):
myTargetMagnitude_deg(angleToRotate_deg),
myCurrentOriR_deg(currentOriR),
mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
mRightMotorVoltageR_mV(rightMotorVoltage_mV)
{
    myMovementCtrl.init(1, AUTOMATIC, -1400 , 1400);
    myMovementCtrl.setTarget( 0 ); // target is the difference between the goal and the actual value
}

void MM::RotationCommandPid::execute()
{
    if( mFinished ) { return; }

    if( !mStarted )
    {
        myTargetOrientation_deg = myCurrentOriR_deg + myTargetMagnitude_deg;
        myTargetOrientation_deg = shiftOrientationValue(myTargetOrientation_deg);
        mStarted = true;
    }

    float orientationDifference_deg = myTargetOrientation_deg - myCurrentOriR_deg;
    orientationDifference_deg = shiftOrientationValue(orientationDifference_deg);

    if( std::abs( orientationDifference_deg ) < 0.5 )
    {
        // TODO: maybe the final motor voltage adjustment should be the responsibility of the caller
        mLeftMotorVoltageR_mV = 0;
        mRightMotorVoltageR_mV = 0;
        mFinished = true;
    }
    else
    {
        myMovementCtrl.compute( static_cast<double>( orientationDifference_deg ) );
        int16_t voltage_mV = static_cast<int16_t>( myMovementCtrl.getOuput() );

        if( voltage_mV > 0 && voltage_mV < CONSTS::K_BIAS_PID_REV )
        {
            voltage_mV = CONSTS::K_BIAS_PID_REV;
        }
        else if ( voltage_mV <= 0 && voltage_mV > -( CONSTS::K_BIAS_PID_REV ) )
        {
            voltage_mV = -( CONSTS::K_BIAS_PID_REV );
        }

        mLeftMotorVoltageR_mV = -voltage_mV;
        mRightMotorVoltageR_mV = voltage_mV;
    }
}

float MM::RotationCommandPid::shiftOrientationValue(float orientationValue)
{
    float retVal = orientationValue;
    if( retVal > 180.0 )
    {
        retVal -= 360;
    }
    else if( retVal < -180.0 )
    {
        retVal += 360;
    }
    return retVal;
}

void MM::RotationCommandPid::print() const
{
    LOG_INFO("GOAL_MAGN: %d GOAL_ORI: %d CURRENT_ORI: %d PID_OUT: %d \n",
        static_cast<int> ( myTargetMagnitude_deg ),
        static_cast<int> ( myTargetOrientation_deg ),
        static_cast<int> ( myCurrentOriR_deg ), 
        static_cast<int> ( myMovementCtrl.getOuput() )
    );
}

MM::CommandResult MM::RotationCommandPid::getResult()
{
    // This is not quite true since we are using the target value and not the measured one, but it is good for now
    // FIXME: If a wrapper is added which can interrupt the rotation, this should be fixed nevertheless!
    return CommandResult(0.0, 0.0, myTargetMagnitude_deg );
}