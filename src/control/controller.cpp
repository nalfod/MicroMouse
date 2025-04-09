// Micromouse course - evosoft

#include "controller.h"
#include "globals.h"


void executeStraightMovementControl()
{
    g.myStraightMovementCtrl.compute( static_cast<double>( g.ir_frontleft - g.ir_frontright ) );
    g.leftMotorVoltage = g.targetMotorVoltage - g.myStraightMovementCtrl.getOuput();
    g.rightMotorVoltage = g.targetMotorVoltage + g.myStraightMovementCtrl.getOuput();
}

void MM::control()
{
    g.targetMotorVoltage = 1000;
    executeStraightMovementControl();
}