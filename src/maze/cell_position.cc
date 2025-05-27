#include "cell_position.h"
#include "constants.h"
#include "utils/logging.h"
MM::CellPosition::CellPosition(float startXPosInCell, float startYPosInCell, CONSTS::Direction startDirection):
mCurrentDirection(startDirection),
mXPositionInCell_mm(startXPosInCell),
mYPositionInCell_mm(startYPosInCell)
{
    
}

void MM::CellPosition::updatePosition(CommandResult movementResult)
{
    movementResult.print();

    if( std::abs(movementResult.dist_traveled_parallel_mm) > 0.0 || std::abs(movementResult.dist_traveled_perpendicular_mm) > 0.0 )
    {
        _updatePositionInCell( movementResult.dist_traveled_parallel_mm, movementResult.dist_traveled_perpendicular_mm );
    }
    
    if( std::abs(movementResult.angle_turned_deg) > 0.0 )
    {
        _updateDirection( CONSTS::adjustAngleToAlignGridDirection( movementResult.angle_turned_deg ) );
        if ( std::abs(movementResult.dist_traveled_parallel_mm) < CONSTS::EPSILON && std::abs(movementResult.dist_traveled_perpendicular_mm) < CONSTS::EPSILON )
        {
            // pure rotation
            _nullPositionInCellAfterRotation();
        }
    }
    printMyself();
}

void MM::CellPosition::_updateDirection(float rotDeg)
{
    CONSTS::Direction newDirection = CONSTS::getDirectionAfterRotation(mCurrentDirection, rotDeg);
    if( newDirection != mCurrentDirection )
    {
        mCurrentDirection = newDirection;
    }
}

void MM::CellPosition::_nullPositionInCellAfterRotation() 
{   
    // This is a huge cheat which basically corrects the behavior that after turning the mouse is not in the middle
    // Now this treats 90 deg and 180 deg turns equally, which is probably not true --> more precise measurement is needed!
    // TODO: refactor, if possible, so this happens through the regular updatePositionInCell
    switch (mCurrentDirection)
    {
        case CONSTS::Direction::NORTH:
            mXPositionInCell_mm = 30.00;
            mYPositionInCell_mm = 0;
            break;
        case CONSTS::Direction::SOUTH:
            mXPositionInCell_mm = -30.00;
            mYPositionInCell_mm = 0;
            break;
        case CONSTS::Direction::EAST:
            mXPositionInCell_mm = 0;
            mYPositionInCell_mm = 30.00;
            break;
        case CONSTS::Direction::WEST:
            mXPositionInCell_mm = 0;
            mYPositionInCell_mm = -30.00;
            break;
        default:
            // Nope
            break;
    }
}

void MM::CellPosition::_updatePositionInCell(float parallel_trav_dist_mm, float perpendicular_trav_dist_mm) 
{
    switch (mCurrentDirection)
    {
        case CONSTS::Direction::NORTH:
            mXPositionInCell_mm += parallel_trav_dist_mm;
            mYPositionInCell_mm += perpendicular_trav_dist_mm;
            break;
        case CONSTS::Direction::SOUTH:
            mXPositionInCell_mm -= parallel_trav_dist_mm;
            mYPositionInCell_mm -= perpendicular_trav_dist_mm;
            break;
        case CONSTS::Direction::EAST:
            mXPositionInCell_mm -= perpendicular_trav_dist_mm;
            mYPositionInCell_mm += parallel_trav_dist_mm;
            break;
        case CONSTS::Direction::WEST:
            mXPositionInCell_mm += perpendicular_trav_dist_mm;
            mYPositionInCell_mm -= parallel_trav_dist_mm;
            break;
        default:
            // Nope
            break;
    }

    // Handle overflow for X and Y positions, possibly for multiple cells
    // TODO: written by copilot, prove if it works!
    while (std::abs(mXPositionInCell_mm) > CONSTS::HALF_CELL_DISTANCE_MM) {
        if (mXPositionInCell_mm > 0) {
            mXPositionInCell_mm -= 2 * CONSTS::HALF_CELL_DISTANCE_MM;
            mPosX += 1;
        } else {
            mXPositionInCell_mm += 2 * CONSTS::HALF_CELL_DISTANCE_MM;
            mPosX -= 1;
        }
    }

    while (std::abs(mYPositionInCell_mm) > CONSTS::HALF_CELL_DISTANCE_MM) {
        if (mYPositionInCell_mm > 0) {
            mYPositionInCell_mm -= 2 * CONSTS::HALF_CELL_DISTANCE_MM;
            mPosY += 1;
        } else {
            mYPositionInCell_mm += 2 * CONSTS::HALF_CELL_DISTANCE_MM;
            mPosY -= 1;
        }
    }

    if( mPosX < 0 ) mPosX = 0;
    if( mPosY < 0 ) mPosY = 0;
}

void MM::CellPosition::updatePositionInCellIfBackwardTouched()
{
    switch (mCurrentDirection)
    {
        case CONSTS::Direction::NORTH:
            mXPositionInCell_mm = - CONSTS::HOME_POSITION_IN_CELL_MM;
            mYPositionInCell_mm = 0;
            break;
        case CONSTS::Direction::SOUTH:
            mXPositionInCell_mm = CONSTS::HOME_POSITION_IN_CELL_MM;
            mYPositionInCell_mm = 0;
            break;
        case CONSTS::Direction::EAST:
            mXPositionInCell_mm = 0;
            mYPositionInCell_mm = - CONSTS::HOME_POSITION_IN_CELL_MM;
            break;
        case CONSTS::Direction::WEST:
            mXPositionInCell_mm = 0;
            mYPositionInCell_mm = CONSTS::HOME_POSITION_IN_CELL_MM;
            break;
        default:
            // Nope
            break;
    }
}

void MM::CellPosition::printMyself()
{
    LOG_INFO("I am in cell x= %d y= %d in the cell: x= %d y= %d ori= %d\n", mPosX, mPosY, 
                                                                            static_cast<int>(mXPositionInCell_mm), 
                                                                            static_cast<int>(mYPositionInCell_mm), 
                                                                            static_cast<int>(mCurrentDirection) );
}