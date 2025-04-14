// Micromouse course - evosoft

#include "controller.h"
#include "globals.h"
#include "arduino.h"
#include "mouse.h"

#include "commands/linear_travel_command.h"
#include "commands/wall_centering_command.h"

void wait2sec()
{
    unsigned long start_ms = millis();
    unsigned long now_ms = start_ms;
    while( now_ms < start_ms + 2000 )
    {
        now_ms = millis();
    }
}

void MM::control()
{
    if( !g.commandArray[g.index]->isFinished() )
    {
        g.commandArray[g.index]->execute();
    }
    else
    {
        g.index++;
        wait2sec();
        if( g.index > 4 )
        {
            mouse.dbg_green.off();
            g.index = 0;
        }
    }
    /*
    if( !g.commandBuffer.empty() )
    {
        if( !g.commandBuffer.front()->isFinished() )
        {
            g.commandBuffer.front()->execute();
        }
        else
        {
            g.commandBuffer.pop();
            
            // if( !g.commandBuffer.empty() )
            // {
            //     g.commandBuffer.front()->execute(); // immediately execute the next command to not waste an cycle
            // }
            wait2sec();
        }
    }
    else
    {
        mouse.dbg_green.off();
    }*/
}