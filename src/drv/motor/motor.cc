#include "motor.h"
#include <cmath>

void MM::Motor::setVoltageAndRotation(int16_t targetVoltage, 
                                      RotationDirection rotationDirection, 
                                      uint16_t currentBatteryVoltage)
{
    float voltageProportion = abs( static_cast<float>( targetVoltage ) / static_cast<float>( currentBatteryVoltage ) );
    uint16_t targetPwm = voltageProportion * 100;

    // triming the pr
    targetPwm = targetPwm > 100 ? 100 : targetPwm;
    targetPwm = targetPwm < -100 ? -100 : targetPwm;

    if( rotationDirection == COUNTER_CLOCKWISE )
    {
        targetPwm *= -1;
    }

    myMotor.motorGoP( targetPwm );
}