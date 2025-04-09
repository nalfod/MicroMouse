#include "motor.h"

void MM::Motor::setVoltageAndRotation(uint16_t targetVoltage, 
                                      RotationDirection rotationDirection, 
                                      uint16_t currentBatteryVoltage)
{
    float voltageProportion = static_cast<float>( targetVoltage ) / static_cast<float>( currentBatteryVoltage );
    uint16_t targetPwm = voltageProportion * 100;

    // triming the pr
    targetPwm = targetPwm > 100 ? 100 : targetPwm;
    targetPwm = targetPwm < 0 ? 0 : targetPwm;

    if( rotationDirection == COUNTER_CLOCKWISE )
    {
        targetPwm *= -1;
    }

    myMotor.motorGoP( targetPwm );
}