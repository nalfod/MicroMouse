#pragma once

#include "constants.h"

class Cell {
    public:
        Cell(int x, int y, int wall, int val = 0);
        Cell(const Cell& cell);

        int getValue() const;
        void setValue(int newValue);

        int getX() const { return mX; }
        int getY() const { return mY; }

        int getWallMask();
        void setWallMask(int newMask);

        bool isAccessible(CONSTS::Direction direction) const ;
    private:
        int mX;
        int mY;

        int wallMask;
        int value;
};