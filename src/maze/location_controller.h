#pragma once

#include "direction.h"
#include "maze.h"
#include "wall_simulator.h"
#include <map>

class LocationController{
    public:
        LocationController(int mazeSize, Direction startDirection, uint16_t& distLeft, uint16_t& distRight, uint16_t& distFrLeft, uint16_t& distFrRight);

        void initMaze(int mazeSize);

        void updateDirection(float rotDeg);
        void moveInDirection(int numOfCells);
        void updateWalls();

        Direction getCurrDirection() const { return mCurrentDirection; }
        float getNextMovement();
    private:
        //SIMULATION
        WallSimulator ws;

        Maze maze;
        Direction mCurrentDirection;

        int mPosX{0};
        int mPosY{0};

        uint16_t& mDistLeft;
        uint16_t& mDistRight;
        uint16_t& mDistFrLeft;
        uint16_t& mDistFrRight;

        std::map<Direction,std::map<float,Direction>> toDirection = 
        { 
            { Direction::NORTH,
                { 
                    {90.0f, Direction::EAST},
                    {180.0f, Direction::SOUTH},
                    {-90.0f, Direction::WEST}
                }
            },
            { Direction::EAST,
                { 
                    {90.0f, Direction::SOUTH},
                    {180.0f, Direction::WEST},
                    {-90.0f, Direction::NORTH}
                }
            },
            { Direction::SOUTH,
                { 
                    {90.0f, Direction::WEST},
                    {180.0f, Direction::NORTH},
                    {-90.0f, Direction::EAST}
                }
            },
            { Direction::WEST,
                { 
                    {90.0f, Direction::NORTH},
                    {180.0f, Direction::EAST},
                    {-90.0f, Direction::SOUTH}
                }
            },
        };
};