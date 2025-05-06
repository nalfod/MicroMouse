#include "cell.h"
#include "direction.h"

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

void Cell::setWallMask(int newMask)
{
    wallMask = newMask;
}

int Cell::getWallMask()
{
    return wallMask;
}

bool Cell::isAccessible(Direction direction) const
{
    return !(wallMask & direction);
}

/*
void Cell::printCell() const
{
    std::string res = "";
    if(wallMask & Direction::WEST) {
        std::cout << "|";
    } else {
        std::cout << " ";
    }

    if(wallMask & Direction::SOUTH) {
        std::cout << "_";
    } else {
        std::cout << " ";
    }

    std::cout <<  value;
    if(value < 10) {
        std::cout << " ";
    }

    if(wallMask & Direction::SOUTH) {
        std::cout <<  "_";
    } else {
        std::cout <<  " ";
    }

    if(wallMask & Direction::EAST) {
        std::cout <<  "|";
    } else {
        std::cout <<  " ";
    }
}*/
