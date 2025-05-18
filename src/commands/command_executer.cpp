#include "command_executer.h"
#include "linear_travel_command.h"
#include "wall_centering_command.h"
#include "collision_avoidance_command.h"
#include "rotation_command_pid.h"
#include "utils/logging.h"

MM::CommandExecuter::CommandExecuter( MM::CellPosition& cellPositionR, 
                                      uint16_t const& dist_left, 
                                      uint16_t const& dist_right, 
                                      uint16_t const& dist_frontleft, 
                                      uint16_t const& dist_frontright, 
                                      int64_t const& encoderValueLeft, 
                                      int64_t const& encoderValueRight, 
                                      float const& currentOriR, 
                                      int16_t& leftMotorVoltage_mV, 
                                      int16_t& rightMotorVoltage_mV):
mCurrentCellPositionR(cellPositionR),
mDistLeftR_mm(dist_left),
mDistRightR_mm(dist_right),
mDistFrontLeftR_mm(dist_frontleft),
mDistFrontRightR_mm(dist_frontright),
encoderValueLeftR_rev(encoderValueLeft),
encoderValueRightR_rev(encoderValueRight),
myCurrentOriR_deg(currentOriR),
mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
mRightMotorVoltageR_mV(rightMotorVoltage_mV)
{

}

void MM::CommandExecuter::execute()
{
    if( mCurrCommandToExecute )
    {
        if( !mCurrCommandToExecute->isFinished() )
        {
            mCurrCommandToExecute->execute();
        }
        else
        {
            mCurrentCellPositionR.updatePosition( mCurrCommandToExecute->getResult() );
            _actualizeCurrentCommand();
        }
    }
    else
    {
        _actualizeCurrentCommand();
    }
}

void MM::CommandExecuter::addCommandRelativeToCurrentPos(int directionToMove_deg, uint16_t numberOfCellsToMove)
{
    if( directionToMove_deg != 0 )
    {
        mCommandsToExecute.push( CommandToExecute(FORWARD_MOVEMENT_FOR_ALIGNMENT, 0) );
        mCommandsToExecute.push( CommandToExecute(ROTATING, directionToMove_deg) );
    }
    mCommandsToExecute.push( CommandToExecute(FORWARD_MOVEMENT_BY_CELL_NUMBER, numberOfCellsToMove ) );
}

bool MM::CommandExecuter::isFinished() const
{
    if( mCurrCommandToExecute )
    {
        return mCurrCommandToExecute->isFinished() && mCommandsToExecute.empty();
    }
    else
    {
        return true;
    }
}

void MM::CommandExecuter::_actualizeCurrentCommand()
{
    if( ( mCurrCommandToExecute == nullptr || mCurrCommandToExecute->isFinished() ) )
    {
        if( !mCommandsToExecute.empty() )
        {
            CommandToExecute paramsOfNextCommand = mCommandsToExecute.front();
            mCommandsToExecute.pop();
            mCurrCommandToExecute = _createCommand(paramsOfNextCommand);
        }
        else
        {
            mCurrCommandToExecute = nullptr;
        }
    }
}

bool MM::CommandExecuter::_isFrontBlocked()
{
    return (mDistLeftR_mm < 80 ) || (mDistRightR_mm < 80 );
}

std::unique_ptr<MM::MotionCommandIF> MM::CommandExecuter::_createCommand(CommandToExecute commandParams)
{
    std::unique_ptr<MM::MotionCommandIF> cmdToReturnP;
    switch ( commandParams.first )
    {
    case FORWARD_MOVEMENT_BY_CELL_NUMBER:
    case FORWARD_MOVEMENT_FOR_ALIGNMENT:
    {
        float distanceToMove_mm = 0.0;
        if( commandParams.first == FORWARD_MOVEMENT_BY_CELL_NUMBER )
        {
            distanceToMove_mm = commandParams.second * CONSTS::HALF_CELL_DISTANCE_MM * 2;
            CONSTS::Direction currentDirectionInCell = mCurrentCellPositionR.getCurrentDirection();

            if( currentDirectionInCell == CONSTS::Direction::NORTH || currentDirectionInCell == CONSTS::Direction::SOUTH )
            {
                if( currentDirectionInCell == CONSTS::Direction::NORTH )
                {
                    distanceToMove_mm -= mCurrentCellPositionR.getXPositionInCell();
                }
                else
                {
                    distanceToMove_mm += mCurrentCellPositionR.getXPositionInCell();
                }
            }
            else if( currentDirectionInCell == CONSTS::Direction::EAST || currentDirectionInCell == CONSTS::Direction::WEST )
            {
                if( currentDirectionInCell == CONSTS::Direction::EAST )
                {
                    distanceToMove_mm -= mCurrentCellPositionR.getYPositionInCell();
                }
                else
                {
                    distanceToMove_mm += mCurrentCellPositionR.getYPositionInCell();
                }
            }
        }
        else if ( commandParams.first == FORWARD_MOVEMENT_FOR_ALIGNMENT )
        {
            if(_isFrontBlocked())
            {
                // front is blocked so it is okay to go one cell, the command will be terminated by the collision avoidance anyway
                distanceToMove_mm = CONSTS::HALF_CELL_DISTANCE_MM * 2;
            }
            else
            {
                distanceToMove_mm = 30;
            }
        }

        cmdToReturnP = std::make_unique<MM::CollisionAvoidanceCommand>
                      ( 
                        std::make_unique<MM::WallCenteringCommand>
                        ( 
                            std::make_unique<MM::LinearTravelCommand>
                            (
                                distanceToMove_mm, 500, 250, 500, encoderValueLeftR_rev, encoderValueRightR_rev, mLeftMotorVoltageR_mV, mRightMotorVoltageR_mV
                            ), 
                            mDistFrontLeftR_mm, mDistFrontRightR_mm, myCurrentOriR_deg, mLeftMotorVoltageR_mV, mRightMotorVoltageR_mV 
                        ),
                        mDistLeftR_mm, mDistRightR_mm, mLeftMotorVoltageR_mV, mRightMotorVoltageR_mV
                      );
        LOG_INFO("NEW LINEAR MOVEMENT CMD: dist= %d dir= %d \n", static_cast<int>(distanceToMove_mm), 
                                                                 static_cast<int>(mCurrentCellPositionR.getCurrentDirection()) );
        break;
    }
    case ROTATING:
    {
        cmdToReturnP = std::make_unique<MM::RotationCommandPid>( commandParams.second, myCurrentOriR_deg, mLeftMotorVoltageR_mV, mRightMotorVoltageR_mV);
        LOG_INFO("NEW ROTATION CMD: deg= %d \n", static_cast<int>(commandParams.second) );
        break;
    }
    default:
        break;
    }

    return std::move(cmdToReturnP);
}