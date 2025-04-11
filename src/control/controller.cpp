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
    //g.leftMotorVoltage = 1000;
    //g.rightMotorVoltage = 500;

    /*g.targetMotorVoltage = 1000;
    executeStraightMovementControl();*/

    if( !g.currentCommand->isFinished() )
    {
        g.currentCommand->execute();
    }
    else
    {
        //delete g.currentCommand;
    }
}