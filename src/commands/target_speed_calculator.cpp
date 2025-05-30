#include "target_speed_calculator.h"
#include "utils/logging.h"

//////////////////////////////////////
////////////// TargetSpeedCalculator
//////////////////////////////////////

MM::TargetSpeedCalculator::TargetSpeedCalculator(float totalDist_mm, 
                                                 float speed_mm_per_s, 
                                                 float acc_mm_per_s2, 
                                                 float dec_mm_per_s2,
                                                 float start_speed_mm_per_s, 
                                                 float end_speed_mm_per_s):
mSetSpeed_mm_per_s(speed_mm_per_s),
mAcceleration_mm_per_s2(acc_mm_per_s2),
mDeceleration_mm_per_s2(dec_mm_per_s2),
mStartSpeed_mm_per_s(start_speed_mm_per_s),
mEndSpeed_mm_per_s(end_speed_mm_per_s)
{
    while( true )
    {
        mAccelerationTime_ms = ( ( mSetSpeed_mm_per_s - mStartSpeed_mm_per_s ) / mAcceleration_mm_per_s2 ) * 1000;
        mDecelerationTime_ms = ( ( mSetSpeed_mm_per_s - mEndSpeed_mm_per_s ) / mDeceleration_mm_per_s2 ) * 1000;

        float accelTime_s = static_cast<float>(mAccelerationTime_ms) / 1000;
        float decelTime_s = static_cast<float>(mDecelerationTime_ms) / 1000;
        float accelDist_mm = mStartSpeed_mm_per_s * accelTime_s + 0.5 * mAcceleration_mm_per_s2 * accelTime_s * accelTime_s;
        float decelDist_mm = mEndSpeed_mm_per_s * decelTime_s + 0.5 * mDeceleration_mm_per_s2 * decelTime_s * decelTime_s;
        long tmpUniformTravelTime_ms =  ( (totalDist_mm - accelDist_mm - decelDist_mm ) / (mSetSpeed_mm_per_s) ) * 1000;

        if( tmpUniformTravelTime_ms > 0 )
        {
            mUniformTravelTime_ms = tmpUniformTravelTime_ms;
            break;
        }
        else
        {
            mSetSpeed_mm_per_s *= 0.9;
            if( mEndSpeed_mm_per_s > mSetSpeed_mm_per_s )
            {
                // end speed shall be less than set speed!
                mEndSpeed_mm_per_s = mSetSpeed_mm_per_s * 0.95;
            }
            
            if( mSetSpeed_mm_per_s <= mStartSpeed_mm_per_s || mSetSpeed_mm_per_s < 1.0 )
            {
                //command params are basically wrongs! --> full decelaration
                mSetSpeed_mm_per_s = mStartSpeed_mm_per_s;
                mAccelerationTime_ms = 0.0;
                mUniformTravelTime_ms = 0.0;

                mDecelerationTime_ms = ((mStartSpeed_mm_per_s - mEndSpeed_mm_per_s) / mDeceleration_mm_per_s2) * 1000;
                if( mDecelerationTime_ms < 0.0 )
                {
                    mDecelerationTime_ms = 0.0;
                }
                break;
            }
        }
    }

    /*LOG_INFO("TargetSpeedCalculator (ctor)-> Total distance(mm): %d Set speed(mm/s): %d Acc time(ms): %d Uni time(ms): %d Dec time(ms): %d\n ",
        static_cast<int>( totalDist_mm ),
        static_cast<int>( mSetSpeed_mm_per_s ),
        static_cast<int>( mAccelerationTime_ms ),
        static_cast<int>( mUniformTravelTime_ms) ,
        static_cast<int>( mDecelerationTime_ms ) );*/
}

float MM::TargetSpeedCalculator::calcCurrentTargetSpeed_mmPerS(unsigned long elapsedTime_ms) const
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
    else
    {
        targetSpeed_mm_per_s = getSpeedInDec_mmPerS( elapsedTime_ms - (mAccelerationTime_ms + mUniformTravelTime_ms) );
    }

    return targetSpeed_mm_per_s;
}

float MM::TargetSpeedCalculator::getSpeedInAcc_mmPerS(unsigned long accElapsedTime_ms) const
{
    return static_cast<float>( accElapsedTime_ms ) / 1000 * mAcceleration_mm_per_s2 + mStartSpeed_mm_per_s;
}

float MM::TargetSpeedCalculator::getSpeedInDec_mmPerS(unsigned long decElapsedTime_ms) const
{
    return mSetSpeed_mm_per_s - (static_cast<float>( decElapsedTime_ms ) / 1000 * mDeceleration_mm_per_s2);
}