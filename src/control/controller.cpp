// Micromouse course - evosoft

#include "controller.h"
#include "globals.h"

void executeStraightMovementControl()
{
    g.myStraightMovementCtrl.compute( static_cast<double>( g.ir_frontleft - g.ir_frontright ) );
    g.leftMotorVoltage = g.leftMotorVoltage - g.myStraightMovementCtrl.getOuput();
    g.rightMotorVoltage = g.rightMotorVoltage + g.myStraightMovementCtrl.getOuput();
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
        executeStraightMovementControl();
    }
    else
    {
        //delete g.currentCommand;
    }
}