// Micromouse course - evosoft

#include "output_handling.h"
#include "mouse.h"
#include "globals.h"

int8_t trim_pwm_signal(int8_t input)
{
    int8_t retval = input;
    retval = retval > 20 ? 20 : retval;
    retval = retval < 10 ? 10 : retval;
    return retval;
}

void MM::post_process()
{
    // g.pwmPercentLeft = trim_pwm_signal(g.pwmPercentLeft);
    // g.pwmPercentLeft = trim_pwm_signal(g.pwmPercentRight);
}

void MM::update_outputs()
{
    //mouse.motor1.motorGoP(g.pwmPercentRight);
    //mouse.motor2.motorGoP(-g.pwmPercentLeft);
}