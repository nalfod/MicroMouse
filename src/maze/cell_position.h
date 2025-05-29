#pragma once
#include "constants.h"
#include "commands/command_result.h"

namespace MM {

class CellPosition
{
    public:
        CellPosition(float startXPosInCell, float startYPosInCell, CONSTS::Direction startDirection);
        // Getters
        CONSTS::Direction getCurrentDirection() const { return mCurrentDirection; }
        int getPosX() const { return mPosX; }
        int getPosY() const { return mPosY; }
        float getXPositionInCell() const { return mXPositionInCell_mm; }
        float getYPositionInCell() const { return mYPositionInCell_mm; }

        // Setters
        void updatePosition(CommandResult movementResult);
        void updatePositionInCellIfBackwardTouched();

        // For debug
        void printMyself();
    private:
        void _updateDirection(float rotDeg);
        void _increasePositionInCell(float traveled_distance_magnitude);
        void _setCurrentDirection(CONSTS::Direction direction);
        

        
        CONSTS::Direction mCurrentDirection{CONSTS::Direction::NORTH};
        int mPosX{0};
        int mPosY{0};
        float mXPositionInCell_mm{0.0};
        float mYPositionInCell_mm{0.0};
};

}