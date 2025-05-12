#pragma once

#include "constants.h"
#include "maze.h"
#include "cell_position.h"

class LocationController{
    public:
        LocationController(int mazeSize, CONSTS::Direction startDirection, uint16_t& distLeft, uint16_t& distRight, uint16_t& distFrLeft, uint16_t& distFrRight);

        void updateWalls();
        bool isFrontWayBlocked();

        float calcNextMovement();

        MM::CellPosition mCurrentPosition;
    private:
        Maze maze;

        bool toMid{true};

        uint16_t& mDistLeft;
        uint16_t& mDistRight;
        uint16_t& mDistFrLeft;
        uint16_t& mDistFrRight;
};