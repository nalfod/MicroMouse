#include "motor.h"
#include <cmath>

MM::Motor::Motor(RotationDirection rotationDirection, int motorPin1, int motorPpin2, int encoderPin1, int encoderPin2): 
myRotationDirection(rotationDirection),
myMotor(motorPin1, motorPpin2) 
{ 
    myEncoder.attachHalfQuad(encoderPin1, encoderPin2); // TODO: revise this, why not full quad??
    myEncoder.clearCount();
}

void MM::Motor::setVoltageAndRotation(int16_t targetVoltage, 
                                      uint16_t currentBatteryVoltage)
{
    float voltageProportion = abs( static_cast<float>( targetVoltage ) / static_cast<float>( currentBatteryVoltage ) );
    uint16_t targetPwm = voltageProportion * 100;

    // triming the pr
    targetPwm = targetPwm > 100 ? 100 : targetPwm;
    targetPwm = targetPwm < -100 ? -100 : targetPwm;

    if( myRotationDirection == COUNTER_CLOCKWISE )
    {
        targetPwm *= -1;
    }

    myMotor.motorGoP( targetPwm );
}

int64_t MM::Motor::getEncoderCount() 
{ 
    int64_t currentCount = myEncoder.getCount();
    if( myRotationDirection == COUNTER_CLOCKWISE )
    {
        currentCount *= -1;
    }
    return currentCount;
}