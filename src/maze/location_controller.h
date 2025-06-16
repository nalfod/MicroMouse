#pragma once

#include "constants.h"
#include "maze.h"
#include "cell_position.h"
#include "wall_detector.h"

class LocationController{
    public:
        LocationController(int mazeSize, MM::CellPosition const& cellPositionR, uint16_t& distLeft, uint16_t& distRight, uint16_t& distFrLeft, uint16_t& distFrRight);

        bool updateWalls();
        int calcNextMovement();

        std::string findRouteForSpeedRun();

        bool isFromMid();
    private:
        MM::CellPosition const& mCurrentPositionR;
        Maze maze;
        MM::WallDetector mWallDetector;
        bool mIsWallUpdateNeeded{false};
        bool toMid{true};

        int numOfFullCircles{0};
};