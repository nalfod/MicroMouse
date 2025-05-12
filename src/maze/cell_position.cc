#include "cell_position.h"
#include "constants.h"

void MM::CellPosition::setCurrentDirection(Direction direction) 
{
    if( direction != mCurrentDirection )
    {
        mPositionInCell_mm = 0.0;
        mCurrentDirection = direction;
    }
}

void MM::CellPosition::increasePositionInCell(float position) 
{ 
    mPositionInCell_mm += position;
    if( mPositionInCell_mm >= CONSTS::CELL_DISTANCE_MM / 2)
    {

    }
}