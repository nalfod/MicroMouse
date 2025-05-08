#pragma once

#include <vector>
#include "cell.h"
#include <queue>

class Maze {
    public:
        Maze(int mazeSize);

        void updateCellWallMask(int x, int y, int wall);

        Direction simpleMove(int currx, int curry);
        void updateMazeValues(int x, int y);

        bool isCellDirectionBlocked(int currx, int curry, Direction dir);

    private:
        void initMaze();

        int calcBaseVal(int x, int y);

        bool isValidPos(int pos);
        void CheckCellandNeighbours();

        std::queue<Cell*> updateQueue;

        std::vector<std::vector<Cell>> cells;

        int numOfRows;
};