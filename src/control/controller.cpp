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
  static bool inDestCell = false;
  // check_mode_selector();
  if( g.mode_selector.get_current_mode() == CONSTS::MODES::DISCOVERY )
  {
    if( !g.commandExecuter.isFinished() )
    {
      g.commandExecuter.execute();
    }
    else
    {
      if(inDestCell)
      {
        inDestCell = false;
        delay(3000);
        if(g.locController.isFromMid())
        {
          LOG_INFO("MID ROTATE\n");
          g.commandExecuter.addRotateAndReorgInCenter();
        }
      }
      if( g.locController.updateWalls() )
      {
        int nextMovement = g.locController.calcNextMovement();
        if( nextMovement == -2 )
        {
          inDestCell = false;
          g.mode_selector.set_current_mode(CONSTS::MODES::SPEED_RUN);
        }
        else if( nextMovement == -3)
        {
          LOG_INFO("MOVE BIT FRONT 1\n");
          g.commandExecuter.addTravelToCenterOfCellCommand();
          inDestCell = true;
        }
        else
        {
          inDestCell = false;
          g.commandExecuter.addTravelCommandRelativeToActualPos( nextMovement , 1);
        }
        /*if( nextMovement != -2)
        {
          g.commandExecuter.addTravelCommandRelativeToActualPos( nextMovement , 1);
        }
        else
        {
          g.mode_selector.set_current_mode(CONSTS::MODES::SPEED_RUN);
        }*/
      }
    }
  }
  else if( g.mode_selector.get_current_mode() == CONSTS::MODES::SPEED_RUN )
  {
    static int safeSpeedRunCounter = 0;
    if( !g.commandExecuter.isFinished() )
    {
      g.commandExecuter.execute();
    }
    else
    {
      if(!inDestCell)
      {
        inDestCell = true;
        LOG_INFO("MOVE BIT FRONT 2\n");
        g.commandExecuter.addTravelToCenterOfCellCommand();
      }
      else
      {
        delay(3000);
        inDestCell = false;
        LOG_INFO("SPEEDRUN_PARSE_START\n");
        if( safeSpeedRunCounter < CONSTS::MODE_RISKY_SPEEDRUN_ACTIVATION_LIMIT )
        {
          g.commandExecuter.parseRouteForSpeedRun(g.locController.findRouteForSpeedRun());
          safeSpeedRunCounter++;
        }
        else
        {
          g.commandExecuter.parseRouteForSpeedRunWithDiagonals(g.locController.findRouteForSpeedRun());
        }
        LOG_INFO("SPEEDRUN_PARSE_END\n");
      }
    }
  }
  else if( g.mode_selector.get_current_mode() == CONSTS::MODES::TESTING )
  {
    if( !g.commandExecuter.isFinished() )
    {
      g.commandExecuter.execute();
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

