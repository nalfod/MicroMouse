// Micromouse course - evosoft

#include "controller.h"
#include "globals.h"

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