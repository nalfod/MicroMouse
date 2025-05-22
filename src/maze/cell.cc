#include "cell.h"

Cell::Cell(int x, int y, int wall, int val):
    mX(x),
    mY(y),
    wallMask(wall),
    value(val),
    wasCellVisited(false)
    {}

Cell::Cell(const Cell& cell) {
    mX = cell.mX;
    mY = cell.mY;
    value = cell.value;
    wallMask = cell.wallMask;
    wasCellVisited = cell.wasCellVisited;
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

bool Cell::isAccessible(CONSTS::Direction direction) const
{
    return !(wallMask & direction);
}

void Cell::isVisited(bool isVisited)
{
    wasCellVisited = isVisited;
}

bool Cell::getWasCellVisited()
{
    return wasCellVisited;
}
