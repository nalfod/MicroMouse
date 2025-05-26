#include "cell.h"
#include "utils/logging.h"
Cell::Cell(int x, int y, int wall, float weight):
    mX(x),
    mY(y),
    mWallMask(wall),
    mWeight(weight),
    mWasCellVisited(false)
    {}

Cell::Cell(const Cell& cell) {
    mX = cell.mX;
    mY = cell.mY;
    mWeight = cell.mWeight;
    mWallMask = cell.mWallMask;
    mWasCellVisited = cell.mWasCellVisited;
}

float Cell::getWeight() const
{
    return mWeight;
}

void Cell::setWeight(float newWeight)
{
    mWeight = newWeight;
    LOG_INFO("WEIGHT OF %d  %d to %F\n", mX, mY, mWeight);
}

int Cell::getWallMask()
{
    return mWallMask;
}

void Cell::setWallMask(int newMask)
{
    mWallMask = newMask;
}

bool Cell::isAccessible(CONSTS::Direction direction) const
{
    return !(mWallMask & direction);
}

void Cell::isVisited(bool isVisited)
{
    mWasCellVisited = isVisited;
}

bool Cell::getWasCellVisited()
{
    return mWasCellVisited;
}
