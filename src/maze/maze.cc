#include "maze.h"
#include <iostream>
#include "direction.h"

#include "utils/logging.h"

Maze::Maze(int mazeSize)
{
    initMaze(mazeSize);
}

int Maze::calcBaseVal(int x, int y) {
    int half = numOfRows/2;
    
    int max = (half-1+(numOfRows%2))*2;
    
    int res = max - x;
    
    if(x > (half-((numOfRows+1)%2))) {
        res = x-((numOfRows+1)%2);
    } else {
        res = max - x;
    }
    
    if(y > (half-((numOfRows+1)%2))) {
        res = res - (max - y) - ((numOfRows+1)%2);
    } else {
        res = res - y;
    }
    
    return res;
}

void Maze::initMaze(int rowNum) {
    numOfRows = rowNum;
    for(int i = 0; i < numOfRows; i++) {
        std::vector<Cell> row;
        row.reserve(10);
        cells.push_back(row);
    }

    for(int i = 0; i < numOfRows; i++) {
        for(int j = 0; j < numOfRows; j++) {
            Cell newCell(i, j, Direction::UNKNOWN, calcBaseVal(i,j));
            cells[i].push_back(newCell);
        }
    }
    cells[0][0].setWallMask(14);
}

void Maze::printMaze() {
    for(int i = numOfRows; i >= 0; i--) {
        for(int j = 0; j < numOfRows; ++j) {
            cells[i][j].printCell();
        }
        std::cout << "\n";
    }
}

void Maze::updateCellWallMask(int x, int y, int wall) {
    LOG_INFO("UPDATEWALL MASK: %d    %d    %d\n",x, y, wall );
    cells[x][y].setWallMask(wall);
}

Direction Maze::simpleMove(int currx, int curry) {
    Cell& c = cells[currx][curry];

    //LOG_INFO("SIMPLEMOVE: %d    %d    %d    %d\n",currx, curry, cells[currx][curry].getValue(), cells[currx][curry].getWallMask() );

    if( c.isAccessible(Direction::NORTH) && isValidPos(currx+1) &&
        (cells[currx+1][curry].getValue() < c.getValue())) {
            return Direction::NORTH;
        }
    
    if( c.isAccessible(Direction::EAST) && isValidPos(curry+1) &&
        (cells[currx][curry+1].getValue() < c.getValue())) {
            return Direction::EAST;
    }

    if( c.isAccessible(Direction::SOUTH) && isValidPos(currx-1) &&
        (cells[currx-1][curry].getValue() < c.getValue())) {
            return Direction::SOUTH;
        }

    if( c.isAccessible(Direction::WEST) && isValidPos(curry-1) &&
        (cells[currx][curry-1].getValue() < c.getValue())) {
            return Direction::WEST;
    }

    return Direction::UNKNOWN;
}

bool Maze::isValidPos(int pos) {
    if((pos > (numOfRows-1)) || (pos < 0)) {
        return false;
    }
    return true;
}

void Maze::updateMazeValues(int x, int y) {
    Cell* currCellP = &cells[x][y];
    updateQueue.push(currCellP);
    while(!updateQueue.empty()) {
        CheckCellandNeighbours();
    }
}

void Maze::CheckCellandNeighbours() {
    Cell*& currCellP = updateQueue.front();

    int lowestNeighbourValue = 20000;

    Cell* cN = 0;
    Cell* cE = 0;
    Cell* cS = 0;
    Cell* cW = 0;
    if( currCellP->isAccessible(Direction::NORTH) && isValidPos(currCellP->getX()+1)) {
        if(cells[currCellP->getX()+1][currCellP->getY()].getValue() < lowestNeighbourValue) {
            lowestNeighbourValue = cells[currCellP->getX()+1][currCellP->getY()].getValue();
        }
        //std::cout << "ADDING1  |" << currCellP->myX+1 << " , " << currCellP->myY << "| TO QUEUE" << std::endl;
        cN = &cells[currCellP->getX()+1][currCellP->getY()];
    }
    if( currCellP->isAccessible(Direction::EAST) && isValidPos(currCellP->getY()+1)) {
        if(cells[currCellP->getX()][currCellP->getY()+1].getValue() < lowestNeighbourValue) {
            lowestNeighbourValue = cells[currCellP->getX()][currCellP->getY()+1].getValue();
        }
        //std::cout << "ADDING2  |" << currCellP->myX << " , " << currCellP->myY+1 << "| TO QUEUE" << std::endl;
        cE = &cells[currCellP->getX()][currCellP->getY()+1];
    }
    if( currCellP->isAccessible(Direction::SOUTH) && isValidPos(currCellP->getX()-1)) {
        if(cells[currCellP->getX()-1][currCellP->getY()].getValue() < lowestNeighbourValue) {
            lowestNeighbourValue = cells[currCellP->getX()-1][currCellP->getY()].getValue();
        }
        //std::cout << "ADDING3  |" << currCellP->myX-1 << " , " << currCellP->myY << "| TO QUEUE" << std::endl;
        cS = &cells[currCellP->getX()-1][currCellP->getY()];
    }
    if( currCellP->isAccessible(Direction::WEST) && isValidPos(currCellP->getY()-1)) {
        if(cells[currCellP->getX()][currCellP->getY()-1].getValue() < lowestNeighbourValue) {
            lowestNeighbourValue = cells[currCellP->getX()][currCellP->getY()-1].getValue();
        }
        //std::cout << "ADDING4  |" << currCellP->myX << " , " << currCellP->myY-1 << "| TO QUEUE" << std::endl;
        cW = &cells[currCellP->getX()][currCellP->getY()-1];
    }
    
    //std::cout << "CHECKING |" << currCellP->myX << " , " << currCellP->myY << " | " << lowestNeighbour << std::endl;
    if(currCellP->getValue() <= lowestNeighbourValue) {
        currCellP->setValue(lowestNeighbourValue+1);

        if(cN != 0) { updateQueue.push(cN);}
        if(cE != 0) { updateQueue.push(cE);}
        if(cS != 0) { updateQueue.push(cS);}
        if(cW != 0) { updateQueue.push(cW);}
    }
    updateQueue.pop();
}