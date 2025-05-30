#pragma once

#include "constants.h"

class Cell {
    public:
        Cell(int x, int y, int wall, float weight = 0);
        Cell(const Cell& cell);

        float getWeight() const;
        void setWeight(float newValue);

        int getX() const { return mX; }
        int getY() const { return mY; }

        int getWallMask();
        void setWallMask(int newMask);

        bool isAccessible(CONSTS::Direction direction) const ;

        void isVisited(bool isVisited);
        bool getWasCellVisited();
    private:
        int mX;
        int mY;

        int mWallMask;
        float mWeight;

        bool mWasCellVisited{false};
};