#pragma once

#include <vector>
#include "cell.h"
#include "constants.h"
#include <queue>
#include <list>

class Maze {
    public:
        Maze(int mazeSize);

        void updateCellWallMask(int x, int y, int wall);

        CONSTS::Direction simpleMove(int currx, int curry);
        void updateMazeValues(int x, int y);

        bool isCellDirectionBlocked(int currx, int curry, CONSTS::Direction dir);

        void resetValues();

        void reCalcMaze(bool toMid);

        float getWeightOfCell(int x, int y);

        void closeMidCells(int x, int y);
        void closeUnknownCells();

        void updateNeighbourWalls(int x, int y);

        std::string findShortestRoute(int x, int y);

        void findShortestRoute2(int x, int y, std::string route, std::vector<std::string>& routes);
        std::string getShortestRoute2(int x, int y);

        void calcForSpeedRun(bool toMid);
        void floodDirectionSpeedRun(CONSTS::Direction checkDirection, int x, int y, CONSTS::Direction currentDir, int sameInRow, float weight, int depth);
    private:
        void initMaze();

        void floodMaze(int x, int y, int stepCount, int d);
        void floodMazeQueue();
        void floodMazeSpeedRun(int x, int y, float weight, int sameInRow, CONSTS::Direction currentDir, int depth);

        int calcBaseVal(int x, int y);

        bool isValidPos(int pos);
        void CheckCellandNeighbours();

        std::queue<Cell*> updateQueue;
        std::list<std::pair<Cell*,int>> updatelist;

        std::vector<std::vector<Cell>> cells;

        int numOfRows;
};