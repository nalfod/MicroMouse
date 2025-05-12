#include "location_controller.h"
#include "Arduino.h"
#include "utils/logging.h"
#include "constants.h"

LocationController::LocationController(int mazeSize, CONSTS::Direction startDirection, uint16_t& distLeft, uint16_t& distRight, uint16_t& distFrLeft, uint16_t& distFrRight):
    mDistLeft(distLeft),
    mDistRight(distRight),
    mDistFrLeft(distFrLeft),
    mDistFrRight(distFrRight),
    maze(mazeSize),
    toMid(true)
{

}

void LocationController::updateWalls()
{
    // TODO FIX
    int newWallMask = 0;
    //LOG_INFO("UPDATE WALLS: %d ---  %d, ---- %d, ----  %d\n",mDistFrLeft, mDistFrRight, mDistLeft, mDistRight );

    if( mDistFrLeft < 90 )
    {
        newWallMask = (newWallMask | CONSTS::DIRECTION_MAP[mCurrentPosition.getCurrentDirection()][-90.0f]);
    }
    if( mDistFrRight < 90 )
    {
        newWallMask = (newWallMask | CONSTS::DIRECTION_MAP[mCurrentPosition.getCurrentDirection()][90.0f]);
    }
    if( (mDistLeft < 80 ) || (mDistRight < 80 ) )
    {
        newWallMask = (newWallMask | mCurrentPosition.getCurrentDirection());
    }

    maze.updateCellWallMask(mCurrentPosition.getPosX(), mCurrentPosition.getPosY(), newWallMask);
}

float LocationController::calcNextMovement()
{
    if(maze.getWeightOfCell(mCurrentPosition.getPosX(),mCurrentPosition.getPosY()) == 0)
    {
        LOG_INFO("REACHED GOAL!!!!!!!!!!RECALC\n");
        toMid = !toMid;
        // We are in the middle and want to move back to the start
        if(!toMid)
        {
            maze.closeMidCells(mCurrentPosition.getPosX(),mCurrentPosition.getPosY());
        }
        maze.reCalcMaze(toMid);
    }
    CONSTS::Direction moveDir = maze.simpleMove(mCurrentPosition.getPosX(), mCurrentPosition.getPosY());
    //LOG_INFO("LOCCONTROL:  %d   %d\n", static_cast<int>(moveDir), static_cast<int>(mCurrentDirection));
    if( moveDir == CONSTS::Direction::UNKNOWN ) {
        maze.updateMazeValues(mCurrentPosition.getPosX(), mCurrentPosition.getPosY());
        moveDir = maze.simpleMove(mCurrentPosition.getPosX(), mCurrentPosition.getPosY());
    }
    if( moveDir == mCurrentPosition.getCurrentDirection()) {
        return 0;
    } else {
        for (auto toDirEntry : CONSTS::DIRECTION_MAP[mCurrentPosition.getCurrentDirection()])
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
    return maze.isCellDirectionBlocked(mCurrentPosition.getPosX(), mCurrentPosition.getPosY(), mCurrentPosition.getCurrentDirection());
}