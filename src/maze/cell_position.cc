#include "cell_position.h"
#include "constants.h"

void MM::CellPosition::updatePosition(CommandResult movementResult)
{
    if( movementResult.distance_traveled_mm > CONSTS::EPSILON && movementResult.angle_turned_deg > CONSTS::EPSILON )
    {
        // Wrong input, only one of the values should be changed at once
        return;
    }

    if( movementResult.distance_traveled_mm > CONSTS::EPSILON )
    {
        _increasePositionInCell( movementResult.distance_traveled_mm );
    }
    else if( movementResult.angle_turned_deg > CONSTS::EPSILON )
    {
        _updateDirection( movementResult.angle_turned_deg );
    }
}

void MM::CellPosition::_setCurrentDirection(CONSTS::Direction direction) 
{
    if( direction != mCurrentDirection )
    {
        mCurrentDirection = direction;
        // This is a huge cheat which basically corrects the behavior that after turning the mouse is not in the middle
        // Now this treats 90 deg and 180 deg turns equally, which is probably not true --> more precise measurement is needed!
        switch (mCurrentDirection)
        {
            case CONSTS::Direction::NORTH:
                mXPositionInCell_mm = 42.00;
                mYPositionInCell_mm = 0;
                break;
            case CONSTS::Direction::SOUTH:
                mXPositionInCell_mm = -42.00;
                mYPositionInCell_mm = 0;
                break;
            case CONSTS::Direction::EAST:
                mXPositionInCell_mm = 0;
                mYPositionInCell_mm = 42.00;
                break;
            case CONSTS::Direction::WEST:
                mXPositionInCell_mm = 0;
                mYPositionInCell_mm = -42.00;
                break;
            default:
                // Nope
                break;
        }
    }
}

void MM::CellPosition::_updateDirection(float rotDeg)
{
    _setCurrentDirection( CONSTS::getDirectionAfterRotation(mCurrentDirection, rotDeg) );
}

void MM::CellPosition::_increasePositionInCell(float traveled_distance_magnitude) 
{
    // NOTE: the function cannot be used with negative traveled ditance, hence the argument name!
    traveled_distance_magnitude = std::abs(traveled_distance_magnitude);

    switch (mCurrentDirection)
    {
        case CONSTS::Direction::NORTH:
            mXPositionInCell_mm += traveled_distance_magnitude;
            break;
        case CONSTS::Direction::SOUTH:
            mXPositionInCell_mm -= traveled_distance_magnitude;
            break;
        case CONSTS::Direction::EAST:
            mYPositionInCell_mm += traveled_distance_magnitude;
            break;
        case CONSTS::Direction::WEST:
            mYPositionInCell_mm -= traveled_distance_magnitude;
            break;
        default:
            // Nope
            break;
    }

    // Handle overflow for X and Y positions, possibly for multiple cells
    // TODO: written by copilot, prove if it works!
    while (std::abs(mXPositionInCell_mm) >= CONSTS::HALF_CELL_DISTANCE_MM) {
        if (mXPositionInCell_mm > 0) {
            mXPositionInCell_mm -= 2 * CONSTS::HALF_CELL_DISTANCE_MM;
            mPosX += 1;
        } else {
            mXPositionInCell_mm += 2 * CONSTS::HALF_CELL_DISTANCE_MM;
            mPosX -= 1;
        }
    }

    while (std::abs(mYPositionInCell_mm) >= CONSTS::HALF_CELL_DISTANCE_MM) {
        if (mYPositionInCell_mm > 0) {
            mYPositionInCell_mm -= 2 * CONSTS::HALF_CELL_DISTANCE_MM;
            mPosY += 1;
        } else {
            mYPositionInCell_mm += 2 * CONSTS::HALF_CELL_DISTANCE_MM;
            mPosY -= 1;
        }
    }
}