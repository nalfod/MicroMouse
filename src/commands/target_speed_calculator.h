#pragma once
#include <cstdint>
#include "constants.h"

namespace MM {

class TargetSpeedCalculator
{
public:
    TargetSpeedCalculator(float totalDist_mm, float speed_mm_per_s, float acc_mm_per_s2, float dec_mm_per_s2, float start_speed_mm_per_s = 0.0, float end_speed_mm_per_s = 0.0);
    float calcCurrentTargetSpeed_mmPerS(unsigned long  elapsedTime_ms) const;
    unsigned long getTotalTimeOfTravel_Ms() const { return mAccelerationTime_ms + mUniformTravelTime_ms + mDecelerationTime_ms; }
    bool isMovementEndsWithStop() const { return mEndSpeed_mm_per_s < CONSTS::EPSILON; }
private:
    float getSpeedInAcc_mmPerS(unsigned long accElapsedTime_ms) const;
    float getSpeedInDec_mmPerS(unsigned long decElapsedTime_ms) const;

    // inputs
    float mSetSpeed_mm_per_s;
    float mAcceleration_mm_per_s2;
    float mDeceleration_mm_per_s2;
    float mStartSpeed_mm_per_s;
    float mEndSpeed_mm_per_s;
    
    // segments of the movement
    unsigned long mAccelerationTime_ms;
    unsigned long mDecelerationTime_ms;
    unsigned long mUniformTravelTime_ms;

};

}