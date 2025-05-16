#pragma once

#include "constants.h"
#include "maze.h"
#include "cell_position.h"

class LocationController{
    public:
        LocationController(int mazeSize, MM::CellPosition const& cellPositionR, uint16_t& distLeft, uint16_t& distRight, uint16_t& distFrLeft, uint16_t& distFrRight);

        void updateWalls();
        void updateWallsBayesian();
        void activateWallUpdateMode() { mIsWallUpdateNeeded = true; }
        bool isInWallUpdateMode() const { return mIsWallUpdateNeeded; }

        bool isFrontWayBlocked();
        int calcNextMovement();

        MM::CellPosition const& mCurrentPositionR;
    private:
        Maze maze;
        bool mIsWallUpdateNeeded{false};
        bool toMid{true};

        uint16_t& mDistLeft;
        uint16_t& mDistRight;
        uint16_t& mDistFrLeft;
        uint16_t& mDistFrRight;

        // --- Bayesian wall probabilities (persist between updates for current cell) ---
        float mProbWallFront1{0.5f};
        float mProbWallFront2{0.5f};
        float mProbWallOnLeft{0.5f};
        float mProbWallOnRight{0.5f};

        // Bayesian helper methods
        void _resetWallProbabilities();
        float _bayesianWallUpdate(float prior, bool detected, float pDetectGivenWall, float pDetectGivenNoWall);
};