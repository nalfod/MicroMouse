#pragma once
#include <cstdint>

namespace MM {

class TargetSpeedCalculator
{
public:
    TargetSpeedCalculator(uint32_t dist_um, uint32_t speed_um_per_ms, uint32_t acc_um_per_ms2, uint32_t dec_um_per_ms2);
    uint32_t calcCurrentTargetSpeed_UmPerMs(unsigned long  elapsedTime_ms);
    unsigned long getTotalTimeOfTravel_Ms() { return mAccelerationTime_ms + mUniformTravelTime_ms + mDecelerationTime_ms; }
private:
    uint32_t getSpeedInAcc_UmPerMs(unsigned long accElapsedTime_ms);
    uint32_t getSpeedInDec_UmPerMs(unsigned long decElapsedTime_ms);

    // inputs
    uint32_t mDistance_um; // um
    uint32_t mSetSpeed_um_per_ms; // um/ms    note: 1 mm/s == 1 um/ms
    uint32_t mAcceleration_um_per_ms2; // um/ms^2    note: 1 mm/s^2 = 0.001 mm/s^2
    uint32_t mDeceleration_um_per_ms2; // um/ms^2
    
    // segments of the movement
    unsigned long mAccelerationTime_ms;
    unsigned long mDecelerationTime_ms;
    unsigned long mUniformTravelTime_ms;

};

}