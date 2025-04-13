// Micromouse course - evosoft

#include "controller.h"
#include "globals.h"
#include "arduino.h"
#include "mouse.h"

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
    if( !g.commandBuffer.empty() )
    {
        if( !g.commandBuffer.front()->isFinished() )
        {
            g.commandBuffer.front()->execute();
        }
        else
        {
            g.commandBuffer.pop();
            /*
            if( !g.commandBuffer.empty() )
            {
                g.commandBuffer.front()->execute(); // immediately execute the next command to not waste an cycle
            }*/
            wait2sec();
        }
    }
    else
    {
        mouse.dbg_green.off();
    }
}