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
    if( mStartSpeed_mm_per_s > mSetSpeed_mm_per_s )
    {
        mSetSpeed_mm_per_s = mStartSpeed_mm_per_s;
    }

    float totalTravelDist_mm = 0.0;
    while( true )
    {
        float tmpAccelTime_s = ( ( mSetSpeed_mm_per_s - mStartSpeed_mm_per_s ) / mAcceleration_mm_per_s2 );
        float tmpDecelTime_s = ( ( mSetSpeed_mm_per_s - mEndSpeed_mm_per_s ) / mDeceleration_mm_per_s2 );
        float tmpAccelDist_mm = mStartSpeed_mm_per_s * tmpAccelTime_s + 0.5 * mAcceleration_mm_per_s2 * tmpAccelTime_s * tmpAccelTime_s;
        float tmpDecelDist_mm = mSetSpeed_mm_per_s * tmpDecelTime_s - 0.5 * mDeceleration_mm_per_s2 * tmpDecelTime_s * tmpDecelTime_s;

        float tmpUniformTravelTime_s =  ( (totalDist_mm - tmpAccelDist_mm - tmpDecelDist_mm ) / (mSetSpeed_mm_per_s) );
        totalTravelDist_mm = tmpAccelDist_mm + tmpDecelDist_mm + (tmpUniformTravelTime_s * mSetSpeed_mm_per_s);

        if( tmpUniformTravelTime_s > 0 )
        {
            mUniformTravelTime_ms = tmpUniformTravelTime_s * 1000;
            mAccelerationTime_ms = tmpAccelTime_s * 1000;
            mDecelerationTime_ms = tmpDecelTime_s * 1000;
            break;
        }
        else
        {
            // 1 . reduce the set speed down until the start speed
            if( mSetSpeed_mm_per_s > mStartSpeed_mm_per_s )
            {
                //LOG_INFO("  TargetSpeedCalculator (ctor)-> 1 . reduce the set speed down until the start speed");
                if( 0.9 * mSetSpeed_mm_per_s >= mStartSpeed_mm_per_s )
                {
                    // setspeed cannot be smaller, than start speed
                    mSetSpeed_mm_per_s *= 0.9;
                }
                else
                {
                    mSetSpeed_mm_per_s = mStartSpeed_mm_per_s;
                }

                if( mEndSpeed_mm_per_s > mSetSpeed_mm_per_s )
                {
                    // end speed shall be less than set speed!
                    mEndSpeed_mm_per_s = mSetSpeed_mm_per_s * 0.99;
                }
                continue;
            }


            // 2. reduce end speed
            if( mEndSpeed_mm_per_s > 0.0 )
            {
                //LOG_INFO("  TargetSpeedCalculator (ctor)-> 2. reduce end speed");
                mEndSpeed_mm_per_s *= 0.9;
                if( mEndSpeed_mm_per_s < 1 )
                {
                    mEndSpeed_mm_per_s = 0.0;
                }
                continue;
            }
            else
            {
                // 3. increase deceleration
                //LOG_INFO("  TargetSpeedCalculator (ctor)-> 3. increase deceleration");
                mDeceleration_mm_per_s2 *= 1.1;
                if( mDeceleration_mm_per_s2 > 5000 )
                {
                    
                    mAccelerationTime_ms = 0.0;
                    mUniformTravelTime_ms = 0.0;

                    mDecelerationTime_ms = (mStartSpeed_mm_per_s / mDeceleration_mm_per_s2) * 1000;
                    if( mDecelerationTime_ms < 0.0 )
                    {
                        mDecelerationTime_ms = 0.0;
                    }
                    break;
                }
            }
        }
    }

    /*LOG_INFO("TargetSpeedCalculator (ctor)-> set dist(mm): %d calc dist(mm): %d mSetSpeed_mm_per_s(mm/s): %d Acc time(ms): %d Uni time(ms): %d Dec time(ms): %d\n ",
        static_cast<int>( totalDist_mm ),
        static_cast<int>( totalTravelDist_mm ),
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