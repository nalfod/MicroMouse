#include "location_controller.h"
#include "Arduino.h"
#include "utils/logging.h"
#include "constants.h"

LocationController::LocationController(int mazeSize, MM::CellPosition const& cellPositionR, uint16_t& distLeft, uint16_t& distRight, uint16_t& distFrLeft, uint16_t& distFrRight):
    mCurrentPositionR(cellPositionR),
    mWallDetector(distLeft, distRight, distFrLeft, distFrRight),
    maze(mazeSize),
    toMid(true),
    numOfFullCircles(0)
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
        int retVal = mWallDetector.getWallMaskOfCurrentCellBayesian( mCurrentPositionR );
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

std::string LocationController::findRouteForSpeedRun()
{
    toMid = !toMid;
    maze.calcForSpeedRun(toMid);
    std::string route = maze.findShortestRoute(mCurrentPositionR.getPosX(),mCurrentPositionR.getPosY());
    if(!toMid)
    {
        switch (mCurrentPositionR.getCurrentDirection())
        {
            case CONSTS::Direction::NORTH:
                route = "NS" + route;
                break;
            case CONSTS::Direction::EAST:
                route = "EW" + route;
            break;
            case CONSTS::Direction::SOUTH:
                route = "SN" + route;
                break;        
            case CONSTS::Direction::WEST:
                route = "WE" + route;
            break;
            default:
                break;
        }
    }

    return route;

    //maze.reCalcMaze(toMid);
    //return maze.getShortestRoute2(mCurrentPositionR.getPosX(),mCurrentPositionR.getPosY());
    //return maze.findShortestRoute(mCurrentPositionR.getPosX(),mCurrentPositionR.getPosY());
}

int LocationController::calcNextMovement()
{
    if(maze.getWeightOfCell(mCurrentPositionR.getPosX(),mCurrentPositionR.getPosY()) == 0)
    {
        toMid = !toMid;
        // We are in the middle and want to move back to the start
        if(!toMid)
        {
            maze.closeMidCells(mCurrentPositionR.getPosX(),mCurrentPositionR.getPosY());
        }
        else
        {
            numOfFullCircles++;
        }
        if(numOfFullCircles >= CONSTS::MODE_SPEED_RUN_ACTIVATION_LIMIT)
        {
            maze.closeUnknownCells();
            toMid = !toMid;
            return -2;
        }
        maze.reCalcMaze(toMid);
        return -3;
    }
    CONSTS::Direction moveDir = maze.simpleMove(mCurrentPositionR.getPosX(), mCurrentPositionR.getPosY());

    if( moveDir == CONSTS::Direction::UNKNOWN ) {
        maze.updateMazeValues(mCurrentPositionR.getPosX(), mCurrentPositionR.getPosY());
        moveDir = maze.simpleMove(mCurrentPositionR.getPosX(), mCurrentPositionR.getPosY());
    }

    return CONSTS::getRotationAngle(mCurrentPositionR.getCurrentDirection(), moveDir);
}

bool LocationController::isFromMid()
{
    return !toMid;
}