// Micromouse course - evosoft

#include "controller.h"
#include "globals.h"
#include <cmath>

const uint16_t baseVoltage = 2500; //in mV

void controlPOfMotors()
{
    float voltageProportion = static_cast<float>( baseVoltage ) / static_cast<float>( g.currentBatteryVoltage );
    uint16_t basePwm = voltageProportion * 100;
    // max: next to the wall: 4000 the other: 400
    // equal: both around 1000
    g.myStraightMovementCtrl.Compute();

    //g.pwmPercentLeft = basePwm - g.output;
    //g.pwmPercentRight = basePwm + g.output;
    g.pwmPercentLeft = basePwm;
    g.pwmPercentRight = basePwm;
}

void MM::control()
{
    g.errorBetweenSides = static_cast<double>( g.ir_frontleft - g.ir_frontright );
    controlPOfMotors();
}