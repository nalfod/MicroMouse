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
    
    //if( g.mode_selector.get_current_mode() == CONSTS::MODES::SPEED_RUN )
    {
        if( !g.commandBuffer.empty() )
        {
            if( !g.commandBuffer.front()->isFinished() )
            {
                //g.commandBuffer.front()->print();
                g.commandBuffer.front()->execute();
            }
            else
            {
                LOG_INFO("COMMAND FINISHED:\n" );
                //g.commandBuffer.front()->print();
                g.commandBuffer.front()->finishCommand();
                g.commandBuffer.pop();
                // if( !g.commandBuffer.empty() )
                // {
                //     g.commandBuffer.front()->execute(); // immediately execute the next command to not waste an cycle
                // }
                //delay(2000);
            }
        }
        else
        {
            LOG_INFO("COMMAND EMPTY:\n" );
            getNextCommand();
            mouse.dbg_green.off();
        }
    }

}

void MM::getNextCommand() 
{
    float nextCommand = g.locController.getNextMovement();
    LOG_INFO("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! NEXT COMMAND: %d\n",static_cast<int>(nextCommand)); 
    if (nextCommand == 0.0f)
    {
        g.commandBuffer.push( 
            std::make_unique<MM::CollisionAvoidanceCommand>
            ( 
              std::make_unique<MM::WallCenteringCommand>
              ( 
                std::make_unique<MM::LinearTravelCommand>
                ( 
                  169500, 100, 1, 1, g.leftEncoderValue, g.rightEncoderValue, g.leftMotorVoltage, g.rightMotorVoltage, g.locController
                ), 
                g.dist_frontleft_mm, g.dist_frontright_mm, g.currentOrientation, g.leftMotorVoltage, g.rightMotorVoltage 
              ),
              g.dist_left_mm, g.dist_right_mm, g.leftMotorVoltage, g.rightMotorVoltage
            )
          );
    }
    else if(nextCommand >= -180.0f && nextCommand <= 180.0f) 
    {
        g.commandBuffer.push( 
            std::make_unique<MM::CollisionAvoidanceCommand>
            ( 
              std::make_unique<MM::WallCenteringCommand>
              ( 
                std::make_unique<MM::LinearTravelCommand>
                ( 
                  169500, 100, 1, 1, g.leftEncoderValue, g.rightEncoderValue, g.leftMotorVoltage, g.rightMotorVoltage, g.locController
                ), 
                g.dist_frontleft_mm, g.dist_frontright_mm, g.currentOrientation, g.leftMotorVoltage, g.rightMotorVoltage 
              ),
              g.dist_left_mm, g.dist_right_mm, g.leftMotorVoltage, g.rightMotorVoltage
            )
          );
        g.commandBuffer.push
        ( 
          std::make_unique<MM::RotationCommandPid>( nextCommand, g.currentOrientation, g.leftMotorVoltage, g.rightMotorVoltage, g.locController)
        );
    }
    else
    {
        LOG_INFO("ERROR COMMAND: %d\n",nextCommand);
    }
}

void MM::check_mode_selector()
{
    // refreshing mode
    /*g.mode_selector.update();
    if( g.mode_selector.is_mode_just_changed() )
    {
        mouse.dbg_green.toggle();
        delay(100);
        mouse.dbg_green.toggle();
        delay(100);
        mouse.dbg_green.toggle();
        delay(100);
        mouse.dbg_green.toggle();
    }*/
}

