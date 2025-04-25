#include "rotation_command_pid.h"
#include <cstdlib>
#include "utils/logging.h"
#include "constants.h"

// DONT USE IT TO TURN MORE THAN 180 (it doesnt make any sense anyway)
MM::RotationCommandPid::RotationCommandPid(RotationOrientation direction, float angleToRotate_deg, float const& currentOriR, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV):
myDircetion(direction),
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
        if( myDircetion == CLOCKWISE )
        {
            myTargetOrientation_deg = myCurrentOriR_deg + myTargetMagnitude_deg;
            if( myTargetOrientation_deg > 180.0 )
            {
                myTargetOrientation_deg -= 360;
            }
        }
        else
        {
            myTargetOrientation_deg = myCurrentOriR_deg - myTargetMagnitude_deg;
            if( myTargetOrientation_deg < -180.0 )
            {
                myTargetOrientation_deg += 360;
            }
        }
        mStarted = true;
    }

    if( std::abs(myTargetOrientation_deg - myCurrentOriR_deg) < 1.0 )
    {
        // TODO: maybe the final motor voltage adjustment should be the responsibility of the caller
        mLeftMotorVoltageR_mV = 0;
        mRightMotorVoltageR_mV = 0;
        mFinished = true;
    }
    else
    {
        double orientationDifference_deg = myTargetOrientation_deg - myCurrentOriR_deg;
        if( orientationDifference_deg < -180 )
        {
            orientationDifference_deg += 360;
        }
        else if( orientationDifference_deg > 180 )
        {
            orientationDifference_deg -= 360;
        }

        myMovementCtrl.compute( orientationDifference_deg );
        int16_t voltage_mV = static_cast<int16_t>( myMovementCtrl.getOuput() );

        if( voltage_mV > 0 && voltage_mV < CONSTS::K_BIAS_FF )
        {
            voltage_mV = CONSTS::K_BIAS_FF;
        }
        else if ( voltage_mV <= 0 && voltage_mV > -CONSTS::K_BIAS_FF )
        {
            voltage_mV = -CONSTS::K_BIAS_FF;
        }

        if( myDircetion == CLOCKWISE )
        {
            mLeftMotorVoltageR_mV = voltage_mV;
            mRightMotorVoltageR_mV = -voltage_mV;
        }
        else
        {
            mLeftMotorVoltageR_mV = -voltage_mV;
            mRightMotorVoltageR_mV = voltage_mV;
        }
    }
}

void MM::RotationCommandPid::print() const
{
    LOG_INFO("GOAL_MAGN: %d GOAL_ORI: %d CURRENT_ORI: %d PID_OUT: %d ",
        static_cast<int> ( myTargetMagnitude_deg ),
        static_cast<int> ( myTargetOrientation_deg ),
        static_cast<int> ( myCurrentOriR_deg ), 
        static_cast<int> ( myMovementCtrl.getOuput() )
    );
}