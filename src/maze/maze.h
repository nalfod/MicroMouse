#pragma once

#include <vector>
#include "cell.h"
#include "constants.h"
#include <queue>

class Maze {
    public:
        Maze(int mazeSize);

        void updateCellWallMask(int x, int y, int wall);

        CONSTS::Direction simpleMove(int currx, int curry);
        void updateMazeValues(int x, int y);

        bool isCellDirectionBlocked(int currx, int curry, CONSTS::Direction dir);

        void resetValues();

        void reCalcMaze(bool toMid);

        int getWeightOfCell(int x, int y);

        void closeMidCells(int x, int y);

        void updateNeighbourWalls(int x, int y);

        std::string findShortestRoute(int x, int y);
    private:
        void initMaze();

        void flowMaze(int x, int y, int stepCount);

        int calcBaseVal(int x, int y);

        bool isValidPos(int pos);
        void CheckCellandNeighbours();

        std::queue<Cell*> updateQueue;

        std::vector<std::vector<Cell>> cells;

        int numOfRows;
};