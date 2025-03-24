// Micromouse course - evosoft

#include "output_handling.h"
#include "mouse.h"

void MM::post_process()
{
    // Your code comes here
}

void MM::update_outputs()
{
    for (int pwm = 0; pwm <= 100; pwm++) { // ramp up forward.
        mouse.motor1.motorGoP(pwm);
        delay(50);
    }
}