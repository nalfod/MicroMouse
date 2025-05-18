#include "location_controller.h"
#include "Arduino.h"
#include "utils/logging.h"
#include "constants.h"

LocationController::LocationController(int mazeSize, MM::CellPosition const& cellPositionR, uint16_t& distLeft, uint16_t& distRight, uint16_t& distFrLeft, uint16_t& distFrRight):
    mCurrentPositionR(cellPositionR),
    mWallDetector(distLeft, distRight, distFrLeft, distFrRight),
    maze(mazeSize),
    toMid(true)
{

}

bool LocationController::updateWalls()
{
    if( 0 )
    {
        // Regular way, measuring only once
        maze.updateCellWallMask( mCurrentPositionR.getPosX(), mCurrentPositionR.getPosY(), mWallDetector.getWallMaskOfCurrentCell( mCurrentPositionR ) );
        return true;
    }
    else
    {
        // Using Bayesian way
        int retVal = mWallDetector.getWallMaskOfCurrentCell( mCurrentPositionR );
        if( retVal >= 0 )
        {
            maze.updateCellWallMask( mCurrentPositionR.getPosX(), mCurrentPositionR.getPosY(), retVal );
            return true;
        }
        else
        {
            return false;
        }
    }
}

int LocationController::calcNextMovement()
{
    if(maze.getWeightOfCell(mCurrentPositionR.getPosX(),mCurrentPositionR.getPosY()) == 0)
    {
        // LOG_INFO("REACHED GOAL!!!!!!!!!!RECALC\n");
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