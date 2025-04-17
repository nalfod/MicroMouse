#include "target_speed_calculator.h"
#include "utils/logging.h"

//////////////////////////////////////
////////////// TargetSpeedCalculator
//////////////////////////////////////

MM::TargetSpeedCalculator::TargetSpeedCalculator(float dist_um, 
                                                 float speed_um_per_ms, 
                                                 float acc_um_per_ms2, 
                                                 float dec_um_per_ms2):
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

    LOG_INFO("TargetSpeedCalculator (ctor)-> Total distance(m): %d Final speed(nm/ms): %d Acc time(ms): %d Uni time(ms): %d Dec time(ms): %d\n ",
        static_cast<int>( mDistance_um ),
        static_cast<int>( mSetSpeed_um_per_ms * 1000 ),
        static_cast<int>( mAccelerationTime_ms ),
        static_cast<int>( mUniformTravelTime_ms) ,
        static_cast<int>( mDecelerationTime_ms ) );
}

float MM::TargetSpeedCalculator::calcCurrentTargetSpeed_UmPerMs(unsigned long elapsedTime_ms)
{
    float targetSpeed_um_per_ms = 0;

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

float MM::TargetSpeedCalculator::getSpeedInAcc_UmPerMs(unsigned long accElapsedTime_ms)
{
    return accElapsedTime_ms * mAcceleration_um_per_ms2;
}

float MM::TargetSpeedCalculator::getSpeedInDec_UmPerMs(unsigned long decElapsedTime_ms)
{
    return mSetSpeed_um_per_ms - (decElapsedTime_ms * mDeceleration_um_per_ms2);
}