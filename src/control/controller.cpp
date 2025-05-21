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
  if( g.mode_selector.get_current_mode() == CONSTS::MODES::DISCOVERY )
  {
    if( !g.commandExecuter.isFinished() )
    {
      g.commandExecuter.execute();
    }
    else
    {
      if( g.locController.updateWalls() )
      {
        LOG_INFO("====NEXT CMD-->\n" );
        int nextMovement = g.locController.calcNextMovement();
        if( nextMovement != -2)
        {
          g.commandExecuter.addCommandRelativeToCurrentPos( nextMovement , 1);
        }
        else
        {
          LOG_INFO("MODE TO SPEEDRUN\n" );
          g.mode_selector.set_current_mode(CONSTS::MODES::SPEED_RUN);
        }
      }
    }
  }
  else if( g.mode_selector.get_current_mode() == CONSTS::MODES::SPEED_RUN )
  {
    if( !g.commandExecuter.isFinished() )
    {
      g.commandExecuter.execute();
    }
    else
    {
      LOG_INFO("PARSE ROUTE SPEEDRUN\n" );
      g.commandExecuter.parseRouteForSpeedRun(g.locController.findRouteForSpeedRun());
      LOG_INFO("PARSE ROUTE SPEEDRUN FINISHED\n" );
      delay(2000);
      // LOG_INFO("COMMAND EXECUTER IS FINISHED:\n" );
    }
  }
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

