// Micromouse course - evosoft

#include "controller.h"
#include "globals.h"
#include <cmath>

const float Kp = 0.005; // Proportional gain, adjust as needed
const uint16_t basePwm = 15; // base speed

void controlPOfMotors()
{
    // max: next to the wall: 4000 the other: 400
    // equal: both around 1000
    uint16_t error = std::abs(g.ir_frontleft - g.ir_frontright);

    uint16_t correction = error * Kp;

    if( g.ir_frontleft > g.ir_frontright )
    {
        g.pwmPercentLeft = basePwm;
        g.pwmPercentRight = basePwm - correction;
        
    }
    else if( g.ir_frontleft < g.ir_frontright )
    {
        g.pwmPercentLeft = basePwm - correction;
        g.pwmPercentRight = basePwm;
    }
    else
    {
        g.pwmPercentLeft = basePwm;
        g.pwmPercentRight = basePwm;
    }

}

void MM::control()
{
    controlPOfMotors();
}