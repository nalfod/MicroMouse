#include "location_controller.h"
#include "Arduino.h"
#include "utils/logging.h"
#include "constants.h"

LocationController::LocationController(int mazeSize, Direction startDirection, uint16_t& distLeft, uint16_t& distRight, uint16_t& distFrLeft, uint16_t& distFrRight):
    mDistLeft(distLeft),
    mDistRight(distRight),
    mDistFrLeft(distFrLeft),
    mDistFrRight(distFrRight),
    mCurrentDirection(startDirection),
    maze(mazeSize)
{}

void LocationController::updateDirection(float rotDeg)
{
    Direction newDir = toDirection[mCurrentDirection][rotDeg];
    //LOG_INFO("CHANGE DIR:  %d   %d\n", static_cast<int>(mCurrentDirection), static_cast<int>(newDir));
    mCurrentDirection = newDir;
}

void LocationController::moveInDirection(int numOfCells)
{
    switch (mCurrentDirection)
    {
    case Direction::NORTH:
        mPosX += numOfCells;
        break;
    case Direction::EAST:
        mPosY += numOfCells;
        break;
    case Direction::SOUTH:
        mPosX -= numOfCells;
        break;
    case Direction::WEST:
        mPosY -= numOfCells;
        break;
    default:
        // Nope
        break;
    }
}

void LocationController::updateWalls()
{
    // TODO FIX
    int newWallMask = 0;
    //LOG_INFO("UPDATE WALLS: %d ---  %d, ---- %d, ----  %d\n",mDistFrLeft, mDistFrRight, mDistLeft, mDistRight );

    if( mDistFrLeft < 90 )
    {
        newWallMask = (newWallMask | toDirection[mCurrentDirection][-90.0f]);
    }
    if( mDistFrRight < 90 )
    {
        newWallMask = (newWallMask | toDirection[mCurrentDirection][90.0f]);
    }
    if( (mDistLeft < 80 ) || (mDistRight < 80 ) )
    {
        newWallMask = (newWallMask | mCurrentDirection);
    }

    maze.updateCellWallMask(mPosX, mPosY, newWallMask);
}

float LocationController::calcNextMovement()
{
    Direction moveDir = maze.simpleMove(mPosX, mPosY);
    //LOG_INFO("LOCCONTROL:  %d   %d\n", static_cast<int>(moveDir), static_cast<int>(mCurrentDirection));
    if( moveDir == Direction::UNKNOWN ) {
        maze.updateMazeValues(mPosX, mPosY);
        moveDir = maze.simpleMove(mPosX, mPosY);
    }
    if( moveDir == mCurrentDirection) {
        return 0;
    } else {
        for (auto toDirEntry : toDirection[mCurrentDirection])
        {
            if(static_cast<int>(toDirEntry.second) == moveDir) {
                return static_cast<int>(toDirEntry.first);
            }
        }
    }
    return -1;
}

bool LocationController::isFrontWayBlocked()
{
    return maze.isCellDirectionBlocked(mPosX, mPosY, mCurrentDirection);
}