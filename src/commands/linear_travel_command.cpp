#include "linear_travel_command.h"
#include "Arduino.h" // for millis()
#include "utils/logging.h"

//////////////////////////////////////
////////////// TargetSpeedCalculator
//////////////////////////////////////

MM::TargetSpeedCalculator::TargetSpeedCalculator(uint32_t dist_um, 
                                                 uint32_t speed_um_per_ms, 
                                                 uint32_t acc_um_per_ms2, 
                                                 uint32_t dec_um_per_ms2):
mDistance_um(dist_um),
mSetSpeed_um_per_ms(speed_um_per_ms),
mAcceleration_um_per_ms2(acc_um_per_ms2),
mDeceleration_um_per_ms2(dec_um_per_ms2)
{
    while( true )
    {
        mAccelerationTime_ms = mSetSpeed_um_per_ms / mAcceleration_um_per_ms2;
        mDecelerationTime_ms = mSetSpeed_um_per_ms / mDeceleration_um_per_ms2;
        mUniformTravelTime_ms  = (mDistance_um - 0.5 * mSetSpeed_um_per_ms * mSetSpeed_um_per_ms * ( 1 / mAcceleration_um_per_ms2 + 1 / mDeceleration_um_per_ms2 ) ) / (mSetSpeed_um_per_ms);

        if( mUniformTravelTime_ms > 0 )
        {
            break;
        }
        else
        {
            mSetSpeed_um_per_ms *= 0.9;
        }
    }

    LOG_INFO("TargetSpeedCalculator (ctor)-> Total distance(um): %d Final speed(um/ms): %d Acc time(ms): %d Uni time(ms): %d Dec time(ms): %d\n ",
        mDistance_um,
        mSetSpeed_um_per_ms,
        mAccelerationTime_ms,
        mUniformTravelTime_ms,
        mDecelerationTime_ms );
}

uint32_t MM::TargetSpeedCalculator::calcCurrentTargetSpeed_UmPerMs(unsigned long elapsedTime_ms)
{
    uint32_t targetSpeed_um_per_ms = 0;

    if( elapsedTime_ms <= mAccelerationTime_ms )
    {
        targetSpeed_um_per_ms = getSpeedInAcc_UmPerMs( elapsedTime_ms );
    }
    else if( elapsedTime_ms > mAccelerationTime_ms && elapsedTime_ms <= ( mAccelerationTime_ms + mUniformTravelTime_ms) )
    {
        targetSpeed_um_per_ms = mSetSpeed_um_per_ms;
    }
    else if( elapsedTime_ms <= (mAccelerationTime_ms + mUniformTravelTime_ms + mDecelerationTime_ms) )
    {
        targetSpeed_um_per_ms = getSpeedInDec_UmPerMs( elapsedTime_ms - (mAccelerationTime_ms + mUniformTravelTime_ms) );
    }
    else
    {
        targetSpeed_um_per_ms = 0;
    }

    return targetSpeed_um_per_ms;
}

uint32_t MM::TargetSpeedCalculator::getSpeedInAcc_UmPerMs(unsigned long accElapsedTime_ms)
{
    return accElapsedTime_ms * mAcceleration_um_per_ms2;
}

uint32_t MM::TargetSpeedCalculator::getSpeedInDec_UmPerMs(unsigned long decElapsedTime_ms)
{
    return mSetSpeed_um_per_ms - (decElapsedTime_ms * mDeceleration_um_per_ms2);
}

////////////////////
///////   LinearTravelCommand
////////////////////

MM::LinearTravelCommand::LinearTravelCommand(uint32_t dist_um, 
                                             uint32_t speed_um_per_ms, 
                                             uint32_t acc_um_per_ms2, 
                                             uint32_t dec_um_per_ms2, 
                                             int64_t const& encoderValue1R, 
                                             int64_t const& encoderValue2R,
                                             int16_t& leftMotorVoltageR_mV,
                                             int16_t& rightMotorVoltageR_mV):
myTargetSpeedCalculator(dist_um, speed_um_per_ms, acc_um_per_ms2, dec_um_per_ms2),
myEncIntegrator1(encoderValue1R),
myEncIntegrator2(encoderValue2R),
mLeftMotorVoltageR_mV(leftMotorVoltageR_mV),
mRightMotorVoltageR_mV(rightMotorVoltageR_mV)
{
    mTotalTimeOfTravel_ms = myTargetSpeedCalculator.getTotalTimeOfTravel_Ms();
    myMovementCtrl.init(1, AUTOMATIC, -1000, 1000);
}


void MM::LinearTravelCommand::execute()
{
    unsigned long now_ms = millis();
    if( !mStarted )
    {
        mStartTime_ms = now_ms;
        mStarted = true;
    }
    
    // determining times
    unsigned long oldElapsedTime_ms = mElapsedTime_ms;
    mElapsedTime_ms = now_ms - mStartTime_ms;
    unsigned long timeChange_ms = mElapsedTime_ms - oldElapsedTime_ms;

    if( mElapsedTime_ms >= mTotalTimeOfTravel_ms )
    {
        mLeftMotorVoltageR_mV = 0;
        mRightMotorVoltageR_mV = 0;
        mFinished = true;
    }
    else
    {
        uint32_t outputSpeed_um_per_ms = myTargetSpeedCalculator.calcCurrentTargetSpeed_UmPerMs( mElapsedTime_ms );
        
        mRealCurrentPosition_um += ( myEncIntegrator1.getTraveledDistanceSinceLastInvoke_Um() + myEncIntegrator2.getTraveledDistanceSinceLastInvoke_Um() ) / 2;
        mDesiredCurrentPosition_um += outputSpeed_um_per_ms * timeChange_ms; 

        /*myMovementCtrl.setTarget( static_cast<double>( mDesiredCurrentPosition_um ) );
        myMovementCtrl.compute( static_cast<double>( mRealCurrentPosition_um) );*/

        int16_t outputVoltage = static_cast<int16_t>(calcVoltageFromSpeed_mV(outputSpeed_um_per_ms)) /*+ static_cast<int16_t>( myMovementCtrl.getOuput() )*/;

        mLeftMotorVoltageR_mV = outputVoltage;
        mRightMotorVoltageR_mV = outputVoltage;

        /* std::cout<<"Elapsed time= " << mElapsedTime_ms << " Current speed= " << outputSpeed_um_per_ms 
                 << " mDesiredCurrentPosition_um= " << mDesiredCurrentPosition_um << std::endl;*/ 
    }
}

int16_t MM::LinearTravelCommand::calcVoltageFromSpeed_mV( int16_t setSpeed_um_per_ms )
{
    return static_cast<int16_t>(K_SPEED_FF * setSpeed_um_per_ms + K_BIAS_FF);
}

