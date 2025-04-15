// Micromouse course - evosoft

#include "controller.h"
#include "globals.h"
#include "arduino.h"
#include "mouse.h"

#include "commands/linear_travel_command.h"
#include "commands/wall_centering_command.h"

void MM::control()
{
    g.leftMotorVoltage = 500;
    g.rightMotorVoltage = -500;
/*    if( !g.commandBuffer.empty() )
    {
        if( !g.commandBuffer.front()->isFinished() )
        {
            // g.commandBuffer.front()->execute();
        }
        else
        {
            g.commandBuffer.pop();
            
            // if( !g.commandBuffer.empty() )
            // {
            //     g.commandBuffer.front()->execute(); // immediately execute the next command to not waste an cycle
            // }
            delay(2000);
        }
    }
    else
    {
        mouse.dbg_green.off();
    }*/
}