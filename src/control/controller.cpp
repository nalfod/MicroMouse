// Micromouse course - evosoft

#include "controller.h"
#include "globals.h"
#include "arduino.h"
#include "mouse.h"

#include "commands/linear_travel_command.h"
#include "commands/wall_centering_command.h"

void MM::control()
{
    // refreshing mode
    g.mode_selector.update();
    if( g.mode_selector.is_mode_just_changed() )
    {
        mouse.dbg_green.toggle();
        delay(100);
        mouse.dbg_green.toggle();
        delay(100);
        mouse.dbg_green.toggle();
        delay(100);
        mouse.dbg_green.toggle();
    }


    if( g.mode_selector.get_current_mode() == CONSTS::MODES::SPEED_RUN )
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
        }
    }

}