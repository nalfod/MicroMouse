#pragma once
#include "constants.h"

namespace MM {

class CellPosition
{
    public:
        // Getters
        CONSTS::Direction getCurrentDirection() const { return mCurrentDirection; }
        int getPosX() const { return mPosX; }
        int getPosY() const { return mPosY; }
        // float getPositionInCell() const { return mPositionInCell_mm; }

        // Setters
        void updateDirection(float rotDeg);
        
        void increasePositionInCell(float traveled);
        // void setPosX(int x) { mPosX = x; }
        // void setPosY(int y) { mPosY = y; }
        //void setPositionInCell(float position) { mPositionInCell_mm = position; }
    private:
        void _setCurrentDirection(CONSTS::Direction direction);
        CONSTS::Direction mCurrentDirection{CONSTS::Direction::NORTH};
        int mPosX{0};
        int mPosY{0};
        float mPositionInCell_mm{0.0};
};

}