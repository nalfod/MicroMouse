#include "location_controller.h"

#include "utils/logging.h"
#include "constants.h"

LocationController::LocationController(int mazeSize, Direction startDirection, uint16_t& distLeft, uint16_t& distRight, uint16_t& distFrLeft, uint16_t& distFrRight):
    mDistLeft(distLeft),
    mDistRight(distRight),
    mDistFrLeft(distFrLeft),
    mDistFrRight(distFrRight),
    mCurrentDirection(startDirection),
    maze(mazeSize)
{
}

void LocationController::updateDirection(float rotDeg)
{
    //LOG_INFO("UPDATEDIR1, next commnad: %d ---  %d\n",static_cast<int>(mCurrentDirection), static_cast<int>(rotDeg));
    mCurrentDirection = toDirection[mCurrentDirection][rotDeg];
    //LOG_INFO("UPDATEDIR2, next commnad: %d ---  %d\n",static_cast<int>(mCurrentDirection), static_cast<int>(rotDeg));
}

void LocationController::moveInDirection(int numOfCells)
{
    switch (mCurrentDirection)
    {
    case Direction::NORTH:
        mPosX++;
        break;
    case Direction::EAST:
        mPosY++;
        break;
    case Direction::SOUTH:
        mPosX--;
        break;
    case Direction::WEST:
        mPosY--;
        break;
    default:
        break;
    }
}

void LocationController::updateWalls()
{
    int newWallMask = 0;
    // TODO FIX
    LOG_INFO("UPDATE WALLS: %d ---  %d, ---- %d, ----  %d\n",mDistFrLeft, mDistFrRight, mDistLeft, mDistRight );
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
    //////// SIMULATOR
    //newWallMask = ws.getData(mPosX,mPosY);
    ////////////

    maze.updateCellWallMask(mPosX, mPosY, newWallMask);
}

float LocationController::getNextMovement()
{
    Direction moveDir = maze.simpleMove(mPosX, mPosY);
    if( moveDir == Direction::UNKNOWN ) {
        maze.updateMazeValues(mPosX, mPosY);
        moveDir = maze.simpleMove(mPosX, mPosY);
    }
    //LOG_INFO("LOCCONTROL, MOVING: %d ---  %d, DIR: %d\n",mPosX, mPosY, moveDir );
    if( moveDir == mCurrentDirection) {
        return 0.0;
    } else {
        for (auto toDirEntry : toDirection[mCurrentDirection])
        {
            //LOG_INFO("GO ENTRIES: %d ---  %d ||| %d\n", mCurrentDirection, static_cast<int>(toDirEntry.second),static_cast<int>(toDirEntry.first));
            if(static_cast<int>(toDirEntry.second) == moveDir) {
                return static_cast<int>(toDirEntry.first);
            }
        }
    }
    return -1;
}