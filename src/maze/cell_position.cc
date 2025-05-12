#include "cell_position.h"
#include "constants.h"

void MM::CellPosition::_setCurrentDirection(CONSTS::Direction direction) 
{
    if( direction != mCurrentDirection )
    {
        mPositionInCell_mm = 0.0;
        mCurrentDirection = direction;
    }
}

void MM::CellPosition::updateDirection(float rotDeg)
{
    _setCurrentDirection( CONSTS::DIRECTION_MAP[mCurrentDirection][rotDeg] );
}

void MM::CellPosition::increasePositionInCell(float position) 
{ 
    mPositionInCell_mm += position;
    if( mPositionInCell_mm >= CONSTS::CELL_DISTANCE_MM / 2)
    {
        mPositionInCell_mm = 0.0;
        switch (mCurrentDirection)
        {
            case CONSTS::Direction::NORTH:
                mPosX += 1;
                break;
            case CONSTS::Direction::EAST:
                mPosY += 1;
                break;
            case CONSTS::Direction::SOUTH:
                mPosX -= 1;
                break;
            case CONSTS::Direction::WEST:
                mPosY -= 1;
                break;
            default:
                // Nope
                break;
        }
    }
}