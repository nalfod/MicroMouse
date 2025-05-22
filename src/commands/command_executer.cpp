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
        mCurrCommandToExecute->execute();
        if( mCurrCommandToExecute->isFinished() )
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
        mCommandsToExecute.push( CommandToExecute(ROTATING_ON_GRID, directionToMove_deg) );
    }
    if( numberOfCellsToMove != 0)
    {
        mCommandsToExecute.push( CommandToExecute(FORWARD_MOVEMENT_BY_CELL_NUMBER, numberOfCellsToMove ) );
    }
}

bool MM::CommandExecuter::isFinished() const
{
    if( mCurrCommandToExecute )
    {
        return mCurrCommandToExecute->isFinished() && mCommandsToExecute.empty();
    }
    else
    {
        return mCommandsToExecute.empty();
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

void  MM::CommandExecuter::parseRouteForSpeedRun(std::string route)
{
    int i = 0;
    CONSTS::Direction currentDir = mCurrentCellPositionR.getCurrentDirection();
    while(i < route.size())
    {
        int16_t moveCellNo = 1;
        CONSTS::Direction toDirection;
        switch (route[i])
        {
            case 'N':
                toDirection = CONSTS::Direction::NORTH;
                break;
            case 'E':
                toDirection = CONSTS::Direction::EAST;
                break;
            case 'S':
                toDirection = CONSTS::Direction::SOUTH;
                break;
            case 'W':
                toDirection = CONSTS::Direction::WEST;
                break;
            default:
                break;
        }
        if(toDirection == currentDir)
        {
            while( i+1 < route.size() && route[i] == route[i+1] )
            {
                i++;
                moveCellNo++;
            }
            i++;
        }
        else
        {
            moveCellNo = 0;
        }
        addCommandRelativeToCurrentPos( CONSTS::getRotationAngle( currentDir, toDirection ), moveCellNo);
        currentDir = toDirection;
    }
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
                distanceToMove_mm = 30; // TODO: measure it!
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
        /*LOG_INFO("NEW LINEAR MOVEMENT CMD: dist= %d dir= %d \n", static_cast<int>(distanceToMove_mm), 
                                                                 static_cast<int>(mCurrentCellPositionR.getCurrentDirection()) );*/
        break;
    }
    case ROTATING:
    {
        cmdToReturnP = std::make_unique<MM::RotationCommandPid>( commandParams.second, myCurrentOriR_deg, mLeftMotorVoltageR_mV, mRightMotorVoltageR_mV);
        //LOG_INFO("NEW ROTATION CMD: deg= %d \n", static_cast<int>(commandParams.second) );
        break;
    }
    case ROTATING_ON_GRID:
    {
        // dest orientation = myCurrentOriR_deg + commandParams.second
        float rawAngleToTurn_deg = commandParams.second;
        float rawDestOrientation_deg = CONSTS::modifyAngleIfCircleOverflow( myCurrentOriR_deg + rawAngleToTurn_deg );
        float alignedDestOrientation_deg = CONSTS::adjustAngleToAlignGridDirection( rawDestOrientation_deg );
        float alignedAngleToTurn_deg = rawAngleToTurn_deg - ( rawDestOrientation_deg - alignedDestOrientation_deg);
        
        ///// DO NOT MERGE THIS, IT NOT WORKS!!!!!
        cmdToReturnP = std::make_unique<MM::RotationCommandPid>( alignedAngleToTurn_deg, myCurrentOriR_deg, mLeftMotorVoltageR_mV, mRightMotorVoltageR_mV);
        LOG_INFO("NEW AL_ROTATION CMD: rawdeg= %d curr_ori= %d al_deg= %d \n", static_cast<int>(rawAngleToTurn_deg), static_cast<int>(myCurrentOriR_deg), static_cast<int>(alignedAngleToTurn_deg) );
        break;
    }
    default:
        break;
    }

    return std::move(cmdToReturnP);
}