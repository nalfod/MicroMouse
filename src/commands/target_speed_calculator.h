#pragma once
#include <cstdint>

namespace MM {

class TargetSpeedCalculator
{
public:
    TargetSpeedCalculator(float dist_mm, float speed_mm_per_s, float acc_mm_per_s2, float dec_mm_per_s2);
    float calcCurrentTargetSpeed_mmPerS(unsigned long  elapsedTime_ms);
    unsigned long getTotalTimeOfTravel_Ms() { return mAccelerationTime_ms + mUniformTravelTime_ms + mDecelerationTime_ms; }
private:
    float getSpeedInAcc_mmPerS(unsigned long accElapsedTime_ms);
    float getSpeedInDec_mmPerS(unsigned long decElapsedTime_ms);

    // inputs
    float mDistance_mm;
    float mSetSpeed_mm_per_s;
    float mAcceleration_mm_per_s2; //note: 1 mm/s^2 = 0.001 mm/s^2
    float mDeceleration_mm_per_s2;
    
    // segments of the movement
    unsigned long mAccelerationTime_ms;
    unsigned long mDecelerationTime_ms;
    unsigned long mUniformTravelTime_ms;

};

}