#include "command_executer.h"
#include "linear_travel_command.h"
#include "wall_centering_command.h"
#include "collision_avoidance_command.h"
#include "rotation_command_pid.h"
#include "arc_travel_command.h"
#include "stuck_avoidance_command.h"
#include "ori_offset_update_command.h"
#include "movement_stabilizers/two_wall_stabilizer.h"
#include "movement_stabilizers/one_wall_stabilizer.h"
#include "movement_stabilizers/orientation_stabilizer.h"

#include "utils/logging.h"
#include <esp32-hal.h>

MM::CommandExecuter::CommandExecuter( MM::CellPosition& cellPositionR, 
                                      uint16_t const& dist_left, 
                                      uint16_t const& dist_right, 
                                      uint16_t const& dist_frontleft, 
                                      uint16_t const& dist_frontright, 
                                      int64_t const& encoderValueLeft, 
                                      int64_t const& encoderValueRight, 
                                      float const& currentOriR, 
                                      int16_t& leftMotorVoltage_mV, 
                                      int16_t& rightMotorVoltage_mV,
                                      bool& oriOffsetFlag):
mCurrentCellPositionR(cellPositionR),
mDistLeftR_mm(dist_left),
mDistRightR_mm(dist_right),
mDistFrontLeftR_mm(dist_frontleft),
mDistFrontRightR_mm(dist_frontright),
encoderValueLeftR_rev(encoderValueLeft),
encoderValueRightR_rev(encoderValueRight),
myCurrentOriR_deg(currentOriR),
mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
mRightMotorVoltageR_mV(rightMotorVoltage_mV),
mOriOffsetFlag(oriOffsetFlag),
mLastOriRefresherTime_ms(millis())
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
            mLastMovementEndSpeed_mm_per_s = mCurrCommandToExecute->getResult().end_speed_mm_per_s;
            _actualizeCurrentCommand();
        }
    }
    else
    {
        _actualizeCurrentCommand();
    }
}

void MM::CommandExecuter::addShortMoveInGoal()
{
    mCommandsToExecute.push( CommandToExecute(FORWARD_MOVEMENT_RAW, 33 ));
}

void MM::CommandExecuter::addRotateAndReorgInCenter()
{
    addTravelCommandRelativeToActualPos(0, 1);
    addTravelCommandRelativeToActualPos(180, 1);
}

void MM::CommandExecuter::addTravelToCenterOfCellCommand()
{
    mCommandsToExecute.push( CommandToExecute(FORWARD_MOVEMENT_TO_CENTER_OF_CELL, 0 ));
}

void MM::CommandExecuter::addTravelCommandRelativeToActualPos(int directionToMove_deg, uint16_t numberOfCellsToMove)
{
    if( directionToMove_deg != 0 )
    {
        mCommandsToExecute.push( CommandToExecute(FORWARD_MOVEMENT_FOR_ROT_ALIGNMENT, 0) );
        mCommandsToExecute.push( CommandToExecute(ROTATING_ON_GRID, directionToMove_deg) );
        
        if( directionToMove_deg > 179.99 || directionToMove_deg < -179.99 )
        {
            // we use the U turn to recenter the mouse!
            mCommandsToExecute.push( CommandToExecute(BACKWARD_MOVEMENT_FOR_ALIGNMENT, CONSTS::HALF_CELL_DISTANCE_MM) );
            mCommandsToExecute.push( CommandToExecute(UPD_ORI_OFFSET_AND_CELL_POS_AT_BACKWALL, 0) );
        }
    }
    if( numberOfCellsToMove != 0)
    {
        mCommandsToExecute.push( CommandToExecute(FORWARD_MOVEMENT_BY_CELL_NUMBER, numberOfCellsToMove ) );
    }
}

void MM::CommandExecuter::addHalfCellTravelCommand()
{
    mCommandsToExecute.push( CommandToExecute(FORWARD_MOVEMENT_RAW, CONSTS::HALF_CELL_DISTANCE_MM) );
}

void MM::CommandExecuter::addArcTravelCommand( float angleToTurn_deg )
{
    mCommandsToExecute.push( CommandToExecute(FORWARD_MOVEMENT_TO_EDGE_OF_CELL, 0) );
    mCommandsToExecute.push( CommandToExecute(ARC_MOVEMENT, angleToTurn_deg) );
    mCommandsToExecute.push( CommandToExecute(FORWARD_MOVEMENT_TO_HOME_IN_CELL, 0) );
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

void MM::CommandExecuter::printActiveCommand() const
{
    if( mCurrCommandToExecute )
    {
        return mCurrCommandToExecute->print();
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
            mCurrCommandToExecute = _createCommandUsingCurrentPosition(paramsOfNextCommand);
        }
        else
        {
            mCurrCommandToExecute = nullptr;
        }
    }
}

bool MM::CommandExecuter::_isFrontBlocked()
{
    // FIXME: would be much nicer to use the current position, but it is not reliable
    // This assumes now, that this function only used when the mouse is at home position
    return (mDistLeftR_mm < 80 + CONSTS::HOME_POSITION_IN_CELL_MM ) || (mDistRightR_mm < 80 + CONSTS::HOME_POSITION_IN_CELL_MM );
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
        addTravelCommandRelativeToActualPos( CONSTS::getRotationAngle( currentDir, toDirection ), moveCellNo);
        currentDir = toDirection;
    }
}

void MM::CommandExecuter::parseRouteForSpeedRunWithDiagonals(std::string route)
{
    int i = 0;
    int16_t moveCellNo = 0;
    CONSTS::Direction currentDir = mCurrentCellPositionR.getCurrentDirection();
    while(i < route.size())
    {
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
            if( i + 1 == route.size())
            {
                moveCellNo++;
                addTravelCommandRelativeToActualPos( 0, moveCellNo );
            }
            i++;
        }
        else
        {
            int firstRotAngle = CONSTS::getRotationAngle( currentDir, toDirection );
            if(firstRotAngle != 180)
            {
                //check next step
                if(i+1 < route.size())
                {
                    CONSTS::Direction tmpDir = toDirection;

                    CONSTS::Direction secondDir;
                    switch (route[i+1])
                    {
                        case 'N':
                            secondDir = CONSTS::Direction::NORTH;
                            break;
                        case 'E':
                            secondDir = CONSTS::Direction::EAST;
                            break;
                        case 'S':
                            secondDir = CONSTS::Direction::SOUTH;
                            break;
                        case 'W':
                            secondDir = CONSTS::Direction::WEST;
                            break;
                        default:
                            break;
                    }
                    if(tmpDir == secondDir)
                    {
                        i++;
                        addTravelCommandRelativeToActualPos( 0, moveCellNo );
                        addArcTravelCommand(firstRotAngle);
                        moveCellNo = 0;
                        currentDir = tmpDir;
                    }
                    else
                    {
                        moveCellNo++;
                        addTravelCommandRelativeToActualPos( 0, moveCellNo );
                        addTravelCommandRelativeToActualPos( firstRotAngle, 0 );
                        currentDir = toDirection;
                        moveCellNo = 0;
                    }
                }
                else
                {
                    i++;
                    addTravelCommandRelativeToActualPos( 0, moveCellNo );
                    addArcTravelCommand(firstRotAngle);
                }
            }
            else
            {
                addTravelCommandRelativeToActualPos( 0, moveCellNo );
                addTravelCommandRelativeToActualPos( 0, 1 );
                addTravelCommandRelativeToActualPos( firstRotAngle, 0 );
                moveCellNo = 0;
                currentDir = toDirection;
            }
        }
    }
}

std::unique_ptr<MM::MotionCommandIF> MM::CommandExecuter::_createCommandUsingCurrentPosition(CommandToExecute commandParams)
{
    LOG_INFO("\n");
    float startSpeed_mm_per_s = 0.0;
    float endSpeed_mm_per_s = 0.0;

    if( mLastMovementEndSpeed_mm_per_s > 0.0  && _isAbleToStartWithSpeed( commandParams.first ) )
    {
        startSpeed_mm_per_s = mLastMovementEndSpeed_mm_per_s;
    }

    if( !mCommandsToExecute.empty() && _isAbleToStartWithSpeed( mCommandsToExecute.front().first ) )
    {
        // LOG_INFO("Current command= %d next command= %d --> end speed is set\n", static_cast<int>(commandParams.first), static_cast<int>(mCommandsToExecute.front().first));
        endSpeed_mm_per_s = CONSTS::END_SPEED_MM_PER_S;
    }

    std::unique_ptr<MM::MotionCommandIF> cmdToReturnP;
    switch ( commandParams.first )
    {
    case FORWARD_MOVEMENT_BY_CELL_NUMBER:
    case FORWARD_MOVEMENT_FOR_ROT_ALIGNMENT:
    case FORWARD_MOVEMENT_TO_EDGE_OF_CELL:
    case FORWARD_MOVEMENT_TO_HOME_IN_CELL:
    case FORWARD_MOVEMENT_TO_CENTER_OF_CELL:
    case FORWARD_MOVEMENT_RAW:
    {
        float distanceToMove_mm = 0.0;
        float currentOffsetInCellInDir = _getOffsetFromHomeInCellInCurrDir();
        if( commandParams.first == FORWARD_MOVEMENT_BY_CELL_NUMBER )
        {
            LOG_INFO("FORWARD_MOVEMENT_BY_CELL_NUMBER\n");
            distanceToMove_mm = commandParams.second * ( 2 * CONSTS::HALF_CELL_DISTANCE_MM ) - currentOffsetInCellInDir;
        }
        else if ( commandParams.first == FORWARD_MOVEMENT_FOR_ROT_ALIGNMENT )
        {
            LOG_INFO("FORWARD_MOVEMENT_FOR_ROT_ALIGNMENT\n");
            
            if( mDistFrontRightR_mm < mDistFrontLeftR_mm )
            {
                mIsRightWallCloser = true;
            }
            else
            {
                mIsRightWallCloser = false;
            }
            LOG_INFO("DIST_RIGHT= %d DIST_LEFT= %d bool= %d\n", static_cast<int>(mDistFrontRightR_mm), static_cast<int>(mDistFrontLeftR_mm), static_cast<int>(mIsRightWallCloser));

            if(_isFrontBlocked())
            {
                // front is blocked so it is okay to go one cell, the command will be terminated by the collision avoidance anyway
                distanceToMove_mm = CONSTS::HALF_CELL_DISTANCE_MM * 2;
            }
            else
            {
                distanceToMove_mm = 40 - currentOffsetInCellInDir; // TODO: measure it!
            }
        }
        else if ( commandParams.first == FORWARD_MOVEMENT_TO_EDGE_OF_CELL )
        {
            LOG_INFO("FORWARD_MOVEMENT_TO_EDGE_OF_CELL\n");
            distanceToMove_mm = CONSTS::HALF_CELL_DISTANCE_MM + ( CONSTS::HOME_POSITION_IN_CELL_MM - currentOffsetInCellInDir );
        }
        else if ( commandParams.first == FORWARD_MOVEMENT_TO_HOME_IN_CELL )
        {
            LOG_INFO("FORWARD_MOVEMENT_TO_HOME_IN_CELL\n");
            if( currentOffsetInCellInDir < 0.0 )
            {
                // we are already in the cell where we want to move home
                distanceToMove_mm = std::abs( currentOffsetInCellInDir );
            }
            else
            {
                // we are in the previous cell
                distanceToMove_mm = ( 2 * CONSTS::HALF_CELL_DISTANCE_MM ) - currentOffsetInCellInDir;
            }
        }
        else if( commandParams.first == FORWARD_MOVEMENT_TO_CENTER_OF_CELL )
        {
            LOG_INFO("FORWARD_MOVEMENT_TO_CENTER_OF_CELL\n");
            distanceToMove_mm = CONSTS::HOME_POSITION_IN_CELL_MM - currentOffsetInCellInDir;
        }
        else if ( commandParams.first == FORWARD_MOVEMENT_RAW )
        {
            LOG_INFO("FORWARD_MOVEMENT_RAW\n");
            distanceToMove_mm = commandParams.second;
        }

        std::vector<std::unique_ptr<MM::MovementStabilizerIF>> stabilizers;
        stabilizers.push_back(std::make_unique<TwoWallStabilizer>(mDistFrontLeftR_mm, mDistFrontRightR_mm));
        stabilizers.push_back(std::make_unique<OneWallStabilizer>(mDistFrontLeftR_mm, mDistFrontRightR_mm));
        stabilizers.push_back(std::make_unique<OrientationStabilizer>(myCurrentOriR_deg, ( ( millis() - mLastOriRefresherTime_ms ) < CONSTS::ORI_REFRESH_DELAY_TOLERANCE_MS ) ) );

        cmdToReturnP = std::make_unique<MM::CollisionAvoidanceCommand>
                      ( 
                        std::make_unique<MM::WallCenteringCommand>
                        ( 
                            std::make_unique<MM::LinearTravelCommand>
                            (
                                distanceToMove_mm, CONSTS::MAX_SPEED_MM_PER_S, CONSTS::ACCELERATION_MM_PER_S2, CONSTS::DECELERATION_MM_PER_S2, startSpeed_mm_per_s, endSpeed_mm_per_s,
                                encoderValueLeftR_rev, encoderValueRightR_rev, mLeftMotorVoltageR_mV, mRightMotorVoltageR_mV
                            ), 
                            std::move( stabilizers ), mLeftMotorVoltageR_mV, mRightMotorVoltageR_mV 
                        ),
                        mDistLeftR_mm, mDistRightR_mm, mLeftMotorVoltageR_mV, mRightMotorVoltageR_mV
                      );
        LOG_INFO("LINEAR MOVEMENT params: dist= %d dir= %d start_speed= %d end_speed= %d\n", static_cast<int>(distanceToMove_mm), 
                                                                                             static_cast<int>(mCurrentCellPositionR.getCurrentDirection()),
                                                                                             static_cast<int>(startSpeed_mm_per_s),
                                                                                             static_cast<int>(endSpeed_mm_per_s) );
        break;
    }
    case ROTATING:
    case ROTATING_ON_GRID:
    {
        float angleToTurn_deg = 0.0;
        if( commandParams.first == ROTATING || ( millis() - mLastOriRefresherTime_ms ) > CONSTS::ORI_REFRESH_DELAY_TOLERANCE_MS )
        {
            angleToTurn_deg = commandParams.second;
            LOG_INFO("ROTATING\n");
        }
        else if( commandParams.first == ROTATING_ON_GRID )
        {
            ///// ATTENTION!! this command assumes that current orientation is updated and eg.: 0 deg means North. But based on experiences, this is not the case
            ///// if the IMU zero point is not updated frequently. Otherwise the etalone orientation will slip eventually, and this command will not rotate to grid directions
            ///// but will cause error. Remark this if you want to use this.

            float rawAngleToTurn_deg = commandParams.second;
            float rawDestOrientation_deg = CONSTS::modifyAngleIfCircleOverflow( myCurrentOriR_deg + rawAngleToTurn_deg );
            float alignedDestOrientation_deg = CONSTS::adjustAngleToAlignGridDirection( rawDestOrientation_deg );
            float alignedAngleToTurn_deg = rawAngleToTurn_deg - ( rawDestOrientation_deg - alignedDestOrientation_deg);
            LOG_INFO("  ROTATING_ON_GRID data: rawdeg= %d curr_ori= %d al_deg= %d \n", static_cast<int>(rawAngleToTurn_deg), static_cast<int>(myCurrentOriR_deg), static_cast<int>(alignedAngleToTurn_deg) );

            angleToTurn_deg = alignedAngleToTurn_deg;
        }

        if( commandParams.second > 179.99 || commandParams.second < -179.99 )
        {
            if( mIsRightWallCloser )
            {
                angleToTurn_deg = std::abs(angleToTurn_deg);
            }
            else
            {
                angleToTurn_deg = -std::abs(angleToTurn_deg);
            }

            if( mCommandsToExecute.size() >= 2 && !_isFrontBlocked() )
            {
                for( int i = 0; i < 2; i++)
                {
                    if( mCommandsToExecute.front().first == BACKWARD_MOVEMENT_FOR_ALIGNMENT ||
                        mCommandsToExecute.front().first == UPD_ORI_OFFSET_AND_CELL_POS_AT_BACKWALL )
                    {
                        mCommandsToExecute.pop();
                    }
                }
            }
        }
        cmdToReturnP = std::make_unique<MM::RotationCommandPid>( angleToTurn_deg, myCurrentOriR_deg, mLeftMotorVoltageR_mV, mRightMotorVoltageR_mV);
        LOG_INFO("NEW ROTATION CMD: deg= %d \n", static_cast<int>(angleToTurn_deg) );
        break;
    }
    case BACKWARD_MOVEMENT_FOR_ALIGNMENT:
    {
        std::vector<std::unique_ptr<MM::MovementStabilizerIF>> stabilizers;
        //stabilizers.push_back(std::make_unique<TwoWallStabilizer>(mDistFrontLeftR_mm, mDistFrontRightR_mm));
        //stabilizers.push_back(std::make_unique<OneWallStabilizer>(mDistFrontLeftR_mm, mDistFrontRightR_mm));
        //stabilizers.push_back(std::make_unique<OrientationStabilizer>(myCurrentOriR_deg));

        cmdToReturnP = std::make_unique<MM::StuckAvoidanceCommand>
                ( 
                    std::make_unique<MM::WallCenteringCommand>
                    ( 
                        std::make_unique<MM::LinearTravelCommand>
                        (
                            (-1 * commandParams.second), 100, 100, 100,
                            encoderValueLeftR_rev, encoderValueRightR_rev, mLeftMotorVoltageR_mV, mRightMotorVoltageR_mV
                        ), 
                        std::move( stabilizers ), mLeftMotorVoltageR_mV, mRightMotorVoltageR_mV 
                    ),
                    encoderValueLeftR_rev, encoderValueLeftR_rev, mLeftMotorVoltageR_mV, mRightMotorVoltageR_mV
                );
        LOG_INFO("NEW BACKWARD_MOVEMENT_FOR_ALIGNMENT CMD: dist= %d \n", static_cast<int>(commandParams.second) );

        break;
    }
    case UPD_ORI_OFFSET_AND_CELL_POS_AT_BACKWALL:
    {
        cmdToReturnP = std::make_unique<MM::OriOffsetUpdater>(mOriOffsetFlag);
        mCurrentCellPositionR.updatePositionInCellIfBackwardTouched();
        mLastOriRefresherTime_ms = millis();
        LOG_INFO("NEW UPD_ORI_OFFSET_AND_CELL_POS_AT_BACKWALL CMD time= %d \n", static_cast<int>( mLastOriRefresherTime_ms ) );
        break;
    }
    case ARC_MOVEMENT:
    {
        cmdToReturnP = std::make_unique<MM::ArcTravelCommand>( CONSTS::HALF_CELL_DISTANCE_MM, 
                                                               commandParams.second, 
                                                               CONSTS::MAX_SPEED_MM_PER_S, 
                                                               CONSTS::ACCELERATION_MM_PER_S2, 
                                                               CONSTS::DECELERATION_MM_PER_S2, 
                                                               startSpeed_mm_per_s, 
                                                               endSpeed_mm_per_s, 
                                                               encoderValueLeftR_rev, encoderValueRightR_rev, 
                                                               mLeftMotorVoltageR_mV, mRightMotorVoltageR_mV);
        LOG_INFO("NEW ARC_MOVEMENT CMD: deg= %d start_speed= %d end_speed= %d \n", static_cast<int>(commandParams.second),
                                                                                   static_cast<int>(startSpeed_mm_per_s),
                                                                                   static_cast<int>(endSpeed_mm_per_s) );
        break;
    }
    default:
        break;
    }

    return std::move(cmdToReturnP);
}

float MM::CommandExecuter::_getOffsetFromHomeInCellInCurrDir()
{
    float offset_mm = 0.0;
    float homePosition_mm = 0.0;
    CONSTS::Direction currentDirectionInCell = mCurrentCellPositionR.getCurrentDirection();

    // Set the home position based on direction
    if (currentDirectionInCell == CONSTS::Direction::NORTH)
    {
        homePosition_mm = - CONSTS::HOME_POSITION_IN_CELL_MM;
        offset_mm = mCurrentCellPositionR.getXPositionInCell() - homePosition_mm;
    }
    else if (currentDirectionInCell == CONSTS::Direction::SOUTH)
    {
        homePosition_mm = CONSTS::HOME_POSITION_IN_CELL_MM;
        offset_mm = homePosition_mm - mCurrentCellPositionR.getXPositionInCell();
    }
    else if (currentDirectionInCell == CONSTS::Direction::EAST)
    {
        homePosition_mm = - CONSTS::HOME_POSITION_IN_CELL_MM;
        offset_mm = mCurrentCellPositionR.getYPositionInCell() - homePosition_mm;
    }
    else if (currentDirectionInCell == CONSTS::Direction::WEST)
    {
        homePosition_mm = CONSTS::HOME_POSITION_IN_CELL_MM;
        offset_mm = homePosition_mm - mCurrentCellPositionR.getYPositionInCell();
    }

    return offset_mm;
}

bool MM::CommandExecuter::_isAbleToStartWithSpeed( MovementPrimitives movementType )
{
    switch (movementType)
    {
        case FORWARD_MOVEMENT_BY_CELL_NUMBER:
        case ARC_MOVEMENT:
        case FORWARD_MOVEMENT_RAW:
        case FORWARD_MOVEMENT_TO_EDGE_OF_CELL:
        case FORWARD_MOVEMENT_TO_HOME_IN_CELL:
        case FORWARD_MOVEMENT_TO_CENTER_OF_CELL:
            return true;
            break;
        case ROTATING:
        case ROTATING_ON_GRID:
        case FORWARD_MOVEMENT_FOR_ROT_ALIGNMENT:
            return false;
            break;
        
        default:
            return false;
            break;
    }
}