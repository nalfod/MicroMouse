#include "cell.h"

Cell::Cell(int x, int y, int wall, int val) {
    mX = x;
    mY = y;
    wallMask = wall;
    value = val;
}

Cell::Cell(const Cell& cell) {
    mX = cell.mX;
    mY = cell.mY;
    value = cell.value;
    wallMask = cell.wallMask;
}

int Cell::getValue() const
{
    return value;
}

void Cell::setValue(int newValue)
{
    value = newValue;
}

int Cell::getWallMask()
{
    return wallMask;
}

void Cell::setWallMask(int newMask)
{
    wallMask = newMask;
}

bool Cell::isAccessible(Direction direction) const
{
    return !(wallMask & direction);
}
