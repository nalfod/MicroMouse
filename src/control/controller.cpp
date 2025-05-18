// Micromouse course - evosoft

#include "controller.h"
#include "globals.h"
#include "arduino.h"
#include "mouse.h"
#include "utils/logging.h"

#include "commands/linear_travel_command.h"
#include "commands/wall_centering_command.h"
#include "commands/collision_avoidance_command.h"
#include "commands/rotation_command_pid.h"

void MM::control()
{
    check_mode_selector();
    /*
    if( g.mode_selector.get_current_mode() == CONSTS::MODES::DISCOVERY )
    {
        if( g.locController.isInWallUpdateMode() )
        {
          mouse.dbg_green.on();
          g.locController.updateWallsBayesian();
        }
        else if( !g.commandBuffer.empty() )
        {
          mouse.dbg_green.on();
          if( !g.commandBuffer.front()->isFinished() )
          {
            //g.commandBuffer.front()->print();
            g.commandBuffer.front()->execute();
          }
          else
          {
              //LOG_INFO("COMMAND FINISHED:\n" );
              //g.commandBuffer.front()->print();
              uint16_t XPosOld = g.currentCellPosition.getPosX();
              uint16_t YPosOld = g.currentCellPosition.getPosY();
              g.currentCellPosition.updatePosition( g.commandBuffer.front()->getResult() );
              g.commandBuffer.pop();

              if( XPosOld != g.currentCellPosition.getPosX() || YPosOld != g.currentCellPosition.getPosY() )
              {
                // We are in a new cell, wall update is necessary!
                g.locController.activateWallUpdateMode();
              }
              // delay(1000);
          }
        }
        else
        {
           // delay(1000);
            // LOG_INFO("COMMAND EMPTY:\n" );
            if( g.mode_selector.get_current_mode() == CONSTS::MODES::DISCOVERY )
            {
              generateNextCommand();
            }
            mouse.dbg_green.on();
        }
    }*/
  g.commandExecuter.execute();
  if( !g.commandExecuter.isFinished() )
  {
    mouse.dbg_green.toggle();
  }
  else
  {
    // LOG_INFO("COMMAND EXECUTER IS FINISHED:\n" );
  }

}

void MM::generateNextCommand() 
{
    g.commandExecuter.addCommandRelativeToCurrentPos( g.locController.calcNextMovement() , 1);
}

void MM::check_mode_selector()
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
}

