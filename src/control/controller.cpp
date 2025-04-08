// Micromouse course - evosoft

#include "controller.h"
#include "globals.h"
#include <cmath>

const float Kp = 0.005; // Proportional gain, adjust as needed
const uint16_t baseVoltage = 1000; //in mV

void controlPOfMotors()
{
    float voltageProportion = static_cast<float>( baseVoltage ) / static_cast<float>( g.currentBatteryVoltage );
    uint16_t basePwm = voltageProportion * 100;
    // max: next to the wall: 4000 the other: 400
    // equal: both around 1000
    g.result = g.myStraightMovementCtrl.Compute();

    g.pwmPercentLeft = basePwm - g.output;
    g.pwmPercentRight = basePwm + g.output; 
}

void MM::control()
{
    g.errorBetweenSides = static_cast<double>( g.ir_frontleft - g.ir_frontright );
    controlPOfMotors();
}