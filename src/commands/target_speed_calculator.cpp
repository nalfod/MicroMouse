#include "target_speed_calculator.h"
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