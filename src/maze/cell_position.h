#pragma once
#include "direction.h"

namespace MM {

class CellPosition
{
    public:
        // Getters
        Direction getCurrentDirection() const { return mCurrentDirection; }
        int getPosX() const { return mPosX; }
        int getPosY() const { return mPosY; }
        // float getPositionInCell() const { return mPositionInCell_mm; }

        // Setters
        void setCurrentDirection(Direction direction) { mCurrentDirection = direction; }
        void setPosX(int x) { mPosX = x; }
        void setPosY(int y) { mPosY = y; }
        //void setPositionInCell(float position) { mPositionInCell_mm = position; }
        void increasePositionInCell(float position);

    private:
        Direction mCurrentDirection{Direction::NORTH};
        int mPosX{0};
        int mPosY{0};
        float mPositionInCell_mm{0.0};
};

}