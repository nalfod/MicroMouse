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
    // Initialize wall probabilities to 0.5 (unknown)
    _resetWallProbabilities();
}

void LocationController::updateWalls()
{
    // TODO FIX
    int newWallMask = 0;
    LOG_INFO("UPDATE WALLS: %d ---  %d, ---- %d, ----  %d\n",mDistFrLeft, mDistFrRight, mDistLeft, mDistRight );

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
    mIsWallUpdateNeeded = false;
}

void LocationController::updateWallsBayesian()
{
    // --- Parameters for Bayesian update ---
    constexpr float pDetectGivenWall = 0.9f;    // Probability sensor detects wall if wall is present
    constexpr float pDetectGivenNoWall = 0.2f;  // Probability sensor detects wall if wall is absent
    constexpr float wallProbabilityThreshold = 0.9f;   // Above this, we are confident there is a wall
    constexpr float noWallProbabilityThreshold = 0.1f; // Below this, we are confident there is no wall

    // --- Sensor model: expected distances for wall detection ---
    constexpr float expectedWallDistanceSide = 90.0f;
    constexpr float expectedWallDistanceFront = 80.0f;

    // --- Bayesian update for each wall direction ---
    // Left wall
    bool detectedOnLeft = (mDistFrLeft < expectedWallDistanceSide);
    mProbWallOnLeft = _bayesianWallUpdate(mProbWallOnLeft, detectedOnLeft, pDetectGivenWall, pDetectGivenNoWall);

    // Right wall
    bool detectedOnRight = (mDistFrRight < expectedWallDistanceSide);
    mProbWallOnRight = _bayesianWallUpdate(mProbWallOnRight, detectedOnRight, pDetectGivenWall, pDetectGivenNoWall);

    // Front wall 1 (relative to current direction)
    bool detectedFront1 = (mDistLeft < expectedWallDistanceFront);
    mProbWallFront1 = _bayesianWallUpdate(mProbWallFront1, detectedFront1, pDetectGivenWall, pDetectGivenNoWall);

    // Right wall (relative to current direction)
    bool detectedFront2 = (mDistRight < expectedWallDistanceFront);
    mProbWallFront2 = _bayesianWallUpdate(mProbWallFront2, detectedFront2, pDetectGivenWall, pDetectGivenNoWall);

    // --- Only update the maze when all probabilities are confident ---
    bool allWallsConfident =
        (mProbWallOnLeft > wallProbabilityThreshold || mProbWallOnLeft < noWallProbabilityThreshold) &&
        (mProbWallOnRight > wallProbabilityThreshold || mProbWallOnRight < noWallProbabilityThreshold) &&
        (mProbWallFront1 > wallProbabilityThreshold || mProbWallFront1 < noWallProbabilityThreshold) &&
        (mProbWallFront2 > wallProbabilityThreshold || mProbWallFront2 < noWallProbabilityThreshold) &&
        (mProbWallFront1 == mProbWallFront2);

    /*LOG_INFO("UPD_WALLS_B: L= %d ---  R= %d, --- F1= %d, ---  F2= %d\n", static_cast<int>( 100 * mProbWallOnLeft ), static_cast<int>( 100 * mProbWallOnRight ), 
                                                                         static_cast<int>( 100 * mProbWallFront1 ), static_cast<int>( 100 * mProbWallFront2 ) );*/
    if (allWallsConfident) 
    {
        int wallMask = 0;

        // If probability is high, set the wall bit for that direction
        if (mProbWallOnLeft > wallProbabilityThreshold) {
            wallMask |= CONSTS::getDirectionAfterRotation(mCurrentPositionR.getCurrentDirection(), -90.0f);
        }
        if (mProbWallOnRight > wallProbabilityThreshold) {
            wallMask |= CONSTS::getDirectionAfterRotation(mCurrentPositionR.getCurrentDirection(), 90.0f);
        }
        if (mProbWallFront1 > wallProbabilityThreshold && mProbWallFront2 > wallProbabilityThreshold) {
            wallMask |= mCurrentPositionR.getCurrentDirection();
        }

        // Commit the wall mask to the maze
        LOG_INFO("UPD_WALLS_B DONE: X= %d ---  Y= %d, --- WM= %d ---", static_cast<int>(mCurrentPositionR.getPosX()), 
                                                                       static_cast<int>(mCurrentPositionR.getPosY()),
                                                                       static_cast<int>(wallMask) );
        LOG_INFO(" L= %d ---  R= %d, --- F1= %d, ---  F2= %d\n", static_cast<int>( 100 * mProbWallOnLeft ), static_cast<int>( 100 * mProbWallOnRight ), 
                                                                 static_cast<int>( 100 * mProbWallFront1 ), static_cast<int>( 100 * mProbWallFront2 ) );
        maze.updateCellWallMask(mCurrentPositionR.getPosX(), mCurrentPositionR.getPosY(), wallMask);
        mIsWallUpdateNeeded = false;

        // Reset probabilities for the next cell
        _resetWallProbabilities();
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

bool LocationController::isFrontWayBlocked()
{
    return maze.isCellDirectionBlocked(mCurrentPositionR.getPosX(), mCurrentPositionR.getPosY(), mCurrentPositionR.getCurrentDirection());
}

void LocationController::_resetWallProbabilities() {
    mProbWallFront1 = 0.5f;
    mProbWallFront2 = 0.5f;
    mProbWallOnLeft = 0.5f;
    mProbWallOnRight = 0.5f;
}

// Bayesian update for a single wall probability
float LocationController::_bayesianWallUpdate(float prior, bool detected, float pDetectGivenWall, float pDetectGivenNoWall) {
    float pWall = prior;
    float pNoWall = 1.0f - prior;

    float likelihoodWall = detected ? pDetectGivenWall : (1.0f - pDetectGivenWall);
    float likelihoodNoWall = detected ? pDetectGivenNoWall : (1.0f - pDetectGivenNoWall);

    float numerator = likelihoodWall * pWall;
    float denominator = numerator + likelihoodNoWall * pNoWall;
    return (denominator > 0.0f) ? numerator / denominator : prior;
}