#pragma once
#include <cstdint>

namespace MM {

class TargetSpeedCalculator
{
public:
    TargetSpeedCalculator(float dist_um, float speed_um_per_ms, float acc_um_per_ms2, float dec_um_per_ms2);
    float calcCurrentTargetSpeed_UmPerMs(unsigned long  elapsedTime_ms);
    unsigned long getTotalTimeOfTravel_Ms() { return mAccelerationTime_ms + mUniformTravelTime_ms + mDecelerationTime_ms; }
private:
    float getSpeedInAcc_UmPerMs(unsigned long accElapsedTime_ms);
    float getSpeedInDec_UmPerMs(unsigned long decElapsedTime_ms);

    // inputs
    float mDistance_um; // um
    float mSetSpeed_um_per_ms; // um/ms    note: 1 mm/s == 1 um/ms
    float mAcceleration_um_per_ms2; // um/ms^2    note: 1 mm/s^2 = 0.001 mm/s^2
    float mDeceleration_um_per_ms2; // um/ms^2
    
    // segments of the movement
    unsigned long mAccelerationTime_ms;
    unsigned long mDecelerationTime_ms;
    unsigned long mUniformTravelTime_ms;

};

}