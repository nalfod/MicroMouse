// Micromouse course - evosoft

#include "output_handling.h"
#include "mouse.h"
#include "globals.h"

void MM::post_process()
{

}

void MM::update_outputs()
{
    mouse.motor1.setVoltageAndRotation(g.rightMotorVoltage, MM::Motor::CLOCKWISE, g.currentBatteryVoltage);
    mouse.motor2.setVoltageAndRotation(g.leftMotorVoltage, MM::Motor::COUNTER_CLOCKWISE, g.currentBatteryVoltage);
}