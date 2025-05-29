#include "wall_detector.h"
#include "utils/logging.h"
#include "constants.h"

MM::WallDetector::WallDetector(uint16_t const& distLeft, uint16_t const& distRight, uint16_t const& distFrLeft, uint16_t const& distFrRight):
    mDistLeft(distLeft),
    mDistRight(distRight),
    mDistFrLeft(distFrLeft),
    mDistFrRight(distFrRight)
{
    // Initialize wall probabilities to 0.5 (unknown)
    _resetWallProbabilities();
}

int MM::WallDetector::getWallMaskOfCurrentCell( CellPosition const& currentPositionR )
{
    // TODO FIX
    int newWallMask = 0;
    //LOG_INFO("UPDATE WALLS: %d ---  %d, ---- %d, ----  %d\n",mDistFrLeft, mDistFrRight, mDistLeft, mDistRight );

    if( mDistFrLeft < 90 )
    {
        newWallMask = (newWallMask | CONSTS::getDirectionAfterRotation(currentPositionR.getCurrentDirection(), -90.0f) );
    }
    if( mDistFrRight < 90 )
    {
        newWallMask = (newWallMask | CONSTS::getDirectionAfterRotation(currentPositionR.getCurrentDirection(), 90.0f) );
    }
    if( (mDistLeft < 80 ) || (mDistRight < 80 ) )
    {
        newWallMask = (newWallMask | currentPositionR.getCurrentDirection());
    }

    return newWallMask;
}

int MM::WallDetector::getWallMaskOfCurrentCellBayesian( CellPosition const& currentPositionR )
{
    int retVal = -1;

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
            wallMask |= CONSTS::getDirectionAfterRotation(currentPositionR.getCurrentDirection(), -90.0f);
        }
        if (mProbWallOnRight > wallProbabilityThreshold) {
            wallMask |= CONSTS::getDirectionAfterRotation(currentPositionR.getCurrentDirection(), 90.0f);
        }
        if (mProbWallFront1 > wallProbabilityThreshold && mProbWallFront2 > wallProbabilityThreshold) {
            wallMask |= currentPositionR.getCurrentDirection();
        }
        
        // Commit the wall mask to the maze
        LOG_INFO("UPD_WALLS_B DONE: X= %d ---  Y= %d, --- WM= %d ---", static_cast<int>(currentPositionR.getPosX()), 
                                                                       static_cast<int>(currentPositionR.getPosY()),
                                                                       static_cast<int>(wallMask) );
        LOG_INFO(" L= %d ---  R= %d, --- F1= %d, ---  F2= %d\n", static_cast<int>( 100 * mProbWallOnLeft ), static_cast<int>( 100 * mProbWallOnRight ), 
                                                                 static_cast<int>( 100 * mProbWallFront1 ), static_cast<int>( 100 * mProbWallFront2 ) );
        
        retVal = wallMask;
        
        // Reset probabilities for the next cell
        _resetWallProbabilities();
    }

    return retVal;
}

void MM::WallDetector::_resetWallProbabilities() {
    mProbWallFront1 = 0.5f;
    mProbWallFront2 = 0.5f;
    mProbWallOnLeft = 0.5f;
    mProbWallOnRight = 0.5f;
}

// Bayesian update for a single wall probability
float MM::WallDetector::_bayesianWallUpdate(float prior, bool detected, float pDetectGivenWall, float pDetectGivenNoWall) {
    float pWall = prior;
    float pNoWall = 1.0f - prior;

    float likelihoodWall = detected ? pDetectGivenWall : (1.0f - pDetectGivenWall);
    float likelihoodNoWall = detected ? pDetectGivenNoWall : (1.0f - pDetectGivenNoWall);

    float numerator = likelihoodWall * pWall;
    float denominator = numerator + likelihoodNoWall * pNoWall;
    return (denominator > 0.0f) ? numerator / denominator : prior;
}