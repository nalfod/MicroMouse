#include "location_controller.h"
#include "Arduino.h"
#include "utils/logging.h"
#include "constants.h"

LocationController::LocationController(int mazeSize, MM::CellPosition const& cellPositionR, uint16_t& distLeft, uint16_t& distRight, uint16_t& distFrLeft, uint16_t& distFrRight):
    mCurrentPositionR(cellPositionR),
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
        newWallMask = (newWallMask | CONSTS::getDirectionAfterRotation(mCurrentPositionR.getCurrentDirection(), -90.0f) );
    }
    if( mDistFrRight < 90 )
    {
        newWallMask = (newWallMask | CONSTS::getDirectionAfterRotation(mCurrentPositionR.getCurrentDirection(), 90.0f) );
    }
    if( (mDistLeft < 80 ) || (mDistRight < 80 ) )
    {
        newWallMask = (newWallMask | mCurrentPositionR.getCurrentDirection());
    }

    maze.updateCellWallMask(mCurrentPositionR.getPosX(), mCurrentPositionR.getPosY(), newWallMask);
}

int LocationController::calcNextMovement()
{
    if(maze.getWeightOfCell(mCurrentPositionR.getPosX(),mCurrentPositionR.getPosY()) == 0)
    {
        LOG_INFO("REACHED GOAL!!!!!!!!!!RECALC\n");
        toMid = !toMid;
        // We are in the middle and want to move back to the start
        if(!toMid)
        {
            maze.closeMidCells(mCurrentPositionR.getPosX(),mCurrentPositionR.getPosY());
        }
        maze.reCalcMaze(toMid);
    }
    CONSTS::Direction moveDir = maze.simpleMove(mCurrentPositionR.getPosX(), mCurrentPositionR.getPosY());
    //LOG_INFO("LOCCONTROL:  %d   %d\n", static_cast<int>(moveDir), static_cast<int>(mCurrentDirection));
    if( moveDir == CONSTS::Direction::UNKNOWN ) {
        maze.updateMazeValues(mCurrentPositionR.getPosX(), mCurrentPositionR.getPosY());
        moveDir = maze.simpleMove(mCurrentPositionR.getPosX(), mCurrentPositionR.getPosY());
    }

    return CONSTS::getRotationAngle(mCurrentPositionR.getCurrentDirection(), moveDir);
}

bool LocationController::isFrontWayBlocked()
{
    return maze.isCellDirectionBlocked(mCurrentPositionR.getPosX(), mCurrentPositionR.getPosY(), mCurrentPositionR.getCurrentDirection());
}