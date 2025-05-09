#include "target_speed_calculator.h"
#include "utils/logging.h"

//////////////////////////////////////
////////////// TargetSpeedCalculator
//////////////////////////////////////

MM::TargetSpeedCalculator::TargetSpeedCalculator(float dist_mm, 
                                                 float speed_mm_per_s, 
                                                 float acc_mm_per_s2, 
                                                 float dec_mm_per_s2):
mDistance_mm(dist_mm),
mSetSpeed_mm_per_s(speed_mm_per_s),
mAcceleration_mm_per_s2(acc_mm_per_s2),
mDeceleration_mm_per_s2(dec_mm_per_s2)
{
    while( true )
    {
        mAccelerationTime_ms = ( mSetSpeed_mm_per_s / mAcceleration_mm_per_s2 ) * 1000;
        mDecelerationTime_ms = ( mSetSpeed_mm_per_s / mDeceleration_mm_per_s2 ) * 1000;
        mUniformTravelTime_ms  = ( (mDistance_mm - 0.5 * mSetSpeed_mm_per_s * mSetSpeed_mm_per_s * ( 1 / mAcceleration_mm_per_s2 + 1 / mDeceleration_mm_per_s2 ) ) / (mSetSpeed_mm_per_s) ) * 1000;

        if( mUniformTravelTime_ms > 0 )
        {
            break;
        }
        else
        {
            mSetSpeed_mm_per_s *= 0.9;
        }
    }

    LOG_INFO("TargetSpeedCalculator (ctor)-> Total distance(mm): %d Final speed(mm/s): %d Acc time(ms): %d Uni time(ms): %d Dec time(ms): %d\n ",
        static_cast<int>( mDistance_mm ),
        static_cast<int>( mSetSpeed_mm_per_s ),
        static_cast<int>( mAccelerationTime_ms ),
        static_cast<int>( mUniformTravelTime_ms) ,
        static_cast<int>( mDecelerationTime_ms ) );
}

float MM::TargetSpeedCalculator::calcCurrentTargetSpeed_mmPerS(unsigned long elapsedTime_ms)
{
    float targetSpeed_mm_per_s = 0;

    if( elapsedTime_ms <= mAccelerationTime_ms )
    {
        targetSpeed_mm_per_s = getSpeedInAcc_mmPerS( elapsedTime_ms );
    }
    else if( elapsedTime_ms > mAccelerationTime_ms && elapsedTime_ms <= ( mAccelerationTime_ms + mUniformTravelTime_ms) )
    {
        targetSpeed_mm_per_s = mSetSpeed_mm_per_s;
    }
    else if( elapsedTime_ms <= (mAccelerationTime_ms + mUniformTravelTime_ms + mDecelerationTime_ms) )
    {
        targetSpeed_mm_per_s = getSpeedInDec_mmPerS( elapsedTime_ms - (mAccelerationTime_ms + mUniformTravelTime_ms) );
    }
    else
    {
        targetSpeed_mm_per_s = 0;
    }

    return targetSpeed_mm_per_s;
}

float MM::TargetSpeedCalculator::getSpeedInAcc_mmPerS(unsigned long accElapsedTime_ms)
{
    return static_cast<float>( accElapsedTime_ms ) / 1000 * mAcceleration_mm_per_s2;
}

float MM::TargetSpeedCalculator::getSpeedInDec_mmPerS(unsigned long decElapsedTime_ms)
{
    return mSetSpeed_mm_per_s - (static_cast<float>( decElapsedTime_ms ) / 1000 * mDeceleration_mm_per_s2);
}