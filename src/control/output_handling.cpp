// Micromouse course - evosoft

#include "output_handling.h"
#include "mouse.h"
#include "globals.h"

void MM::post_process()
{
    g.pwmPercentLeft = g.pwmPercentLeft > 20 ? 20 : g.pwmPercentLeft;
    g.pwmPercentLeft = g.pwmPercentLeft < 10 ? 10 : g.pwmPercentLeft;
    g.pwmPercentRight = g.pwmPercentRight > 20 ? 20 : g.pwmPercentRight;
    g.pwmPercentRight = g.pwmPercentRight < 10 ? 10 : g.pwmPercentRight;
}

void MM::update_outputs()
{
    mouse.motor1.motorGoP(g.pwmPercentRight);
    mouse.motor2.motorGoP(-g.pwmPercentLeft);
}