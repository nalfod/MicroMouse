#pragma once

#include "Direction.h"

class Cell {
    public:
        Cell(int x, int y, int wall, int val = 0);
        Cell(const Cell& cell);

        void printCell() const;

        int getValue() const;
        void setValue(int newValue);

        int getX() const { return mX; }
        int getY() const { return mY; }


        void setWallMask(int newMask);
        int getWallMask();
        bool isAccessible(Direction direction) const ;
    private:
        int mX;
        int mY;

        int wallMask;
        int value;
};