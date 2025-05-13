#include "maze.h"
#include <iostream>
#include "direction.h"

#include "utils/logging.h"

Maze::Maze(int mazeSize)
{
    numOfRows = mazeSize;
    initMaze();
}

void Maze::initMaze() {
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

int Maze::calcBaseVal(int x, int y) {
    int half = numOfRows/2;
    int maxVal = ( ( half - 1 + (numOfRows % 2) ) * 2 );
    
    int res = maxVal - x;
    
    if(x > ( half - ( (numOfRows+1) % 2) ) ) 
    {
        res = x - ( ( numOfRows + 1 ) % 2 );
    } 
    else 
    {
        res = maxVal - x;
    }
    
    if(y > ( half - ( ( numOfRows + 1 ) % 2 ) ) ) 
    {
        res = res - ( maxVal - y ) - ( ( numOfRows + 1 ) % 2 );
    } 
    else 
    {
        res -= y;
    }
    
    return res;
}

void Maze::updateCellWallMask(int x, int y, int wall) {
    // PRINT MAZE
    /*for(int i = 6; i >= 0; i--) {
        for(int j = 0; j < 7; j++ ) {
            LOG_INFO(" %d ", cells[i][j].getValue());
        }
        LOG_INFO("\n");
    }*/

    //LOG_INFO("UPDATEWALL MASK: %d    %d    %d\n",x, y, wall );
    cells[x][y].setWallMask(wall);
    updateNeighbourWalls(x,y);
}

void Maze::updateNeighbourWalls(int x, int y)
{
    if( !cells[x][y].isAccessible(Direction::NORTH) && isValidPos(x+1))
    {
        int newWallMask = (cells[x+1][y].getWallMask() | Direction::SOUTH);
        cells[x+1][y].setWallMask(newWallMask);
    }
    if( !cells[x][y].isAccessible(Direction::EAST) && isValidPos(y+1))
    {
        int newWallMask = (cells[x][y+1].getWallMask() | Direction::WEST);
        cells[x][y+1].setWallMask(newWallMask);
    }
    if( !cells[x][y].isAccessible(Direction::SOUTH) && isValidPos(x-1))
    {
        int newWallMask = (cells[x-1][y].getWallMask() | Direction::NORTH);
        cells[x-1][y].setWallMask(newWallMask);
    }
    if( !cells[x][y].isAccessible(Direction::WEST) && isValidPos(y-1))
    {
        int newWallMask = (cells[x][y-1].getWallMask() | Direction::EAST);
        cells[x][y-1].setWallMask(newWallMask);
    }
}

Direction Maze::simpleMove(int currx, int curry) {
    Cell& c = cells[currx][curry];
    if(currx == 6 && curry == 3) {
        /*LOG_INFO("CURRENT VAL:  %d\n",cells[currx][curry].getValue());
        LOG_INFO("N:  %d\n", c.isAccessible(Direction::NORTH));
        LOG_INFO("E:  %d   %d\n", c.isAccessible(Direction::EAST), cells[currx][curry+1].getValue());
        LOG_INFO("S:  %d   %d\n", c.isAccessible(Direction::SOUTH), cells[currx-1][curry].getValue());
        LOG_INFO("W:  %d   %d\n", c.isAccessible(Direction::WEST), cells[currx][curry-1].getValue());*/
    }

    if( c.isAccessible(Direction::NORTH) && isValidPos(currx+1) &&
        (cells[currx+1][curry].getValue() < c.getValue())) 
    {
        return Direction::NORTH;
    }
    
    if( c.isAccessible(Direction::EAST) && isValidPos(curry+1) &&
        (cells[currx][curry+1].getValue() < c.getValue())) 
    {
        return Direction::EAST;
    }

    if( c.isAccessible(Direction::SOUTH) && isValidPos(currx-1) &&
        (cells[currx-1][curry].getValue() < c.getValue())) 
    {
        return Direction::SOUTH;
    }

    if( c.isAccessible(Direction::WEST) && isValidPos(curry-1) &&
        (cells[currx][curry-1].getValue() < c.getValue())) 
    {
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
    
    if( currCellP->isAccessible(Direction::NORTH) && isValidPos(currCellP->getX()+1)) 
    {
        if(cells[currCellP->getX()+1][currCellP->getY()].getValue() < lowestNeighbourValue) 
        {
            lowestNeighbourValue = cells[currCellP->getX()+1][currCellP->getY()].getValue();
        }
        cN = &cells[currCellP->getX()+1][currCellP->getY()];
    }
    if( currCellP->isAccessible(Direction::EAST) && isValidPos(currCellP->getY()+1)) 
    {
        if(cells[currCellP->getX()][currCellP->getY()+1].getValue() < lowestNeighbourValue) 
        {
            lowestNeighbourValue = cells[currCellP->getX()][currCellP->getY()+1].getValue();
        }
        cE = &cells[currCellP->getX()][currCellP->getY()+1];
    }
    if( currCellP->isAccessible(Direction::SOUTH) && isValidPos(currCellP->getX()-1)) 
    {
        if(cells[currCellP->getX()-1][currCellP->getY()].getValue() < lowestNeighbourValue) 
        {
            lowestNeighbourValue = cells[currCellP->getX()-1][currCellP->getY()].getValue();
        }
        cS = &cells[currCellP->getX()-1][currCellP->getY()];
    }
    if( currCellP->isAccessible(Direction::WEST) && isValidPos(currCellP->getY()-1)) 
    {
        if(cells[currCellP->getX()][currCellP->getY()-1].getValue() < lowestNeighbourValue) 
        {
            lowestNeighbourValue = cells[currCellP->getX()][currCellP->getY()-1].getValue();
        }
        cW = &cells[currCellP->getX()][currCellP->getY()-1];
    }
    
    if(currCellP->getValue() <= lowestNeighbourValue) 
    {
        currCellP->setValue(lowestNeighbourValue+1);

        if(cN != 0) { updateQueue.push(cN);}
        if(cE != 0) { updateQueue.push(cE);}
        if(cS != 0) { updateQueue.push(cS);}
        if(cW != 0) { updateQueue.push(cW);}
    }
    updateQueue.pop();
}

bool Maze::isCellDirectionBlocked(int currx, int curry, Direction dir)
{
    return (0 != (cells[currx][curry].getWallMask() & dir));
}

void Maze::resetValues()
{
    for(int i = 0; i < numOfRows; i++)
    {
        for(int j = 0; j < numOfRows; j++)
        {
            cells[i][j].setValue(20000);
        }
    }
}

void Maze::reCalcMaze(bool toMid)
{
    resetValues();
    if(toMid)
    {
        if(numOfRows%2 != 0)
        {
            flowMaze(numOfRows/2,numOfRows/2,0);
        }
        else
        {
            int posx = 0;
            int posy = 0;
            for(int i = 0; i < 2; i++)
            {
                for(int j = 0; j < 2; j++)
                {
                    if ( cells[numOfRows/2 - 1 + i][numOfRows/2 - 1 + j].getWallMask() < 15) {
                        posx = numOfRows/2 - 1 + i;
                        posy = numOfRows/2 - 1 + j;
                    }
                }
            }
            std::cout << "FLOWMAZE ODD " << posx << "  " << posy << std::endl;
            flowMaze(posx,posy,0);
            return;
        }
    }
    else
    {
        flowMaze(0,0,0);
    }
}

void Maze::closeMidCells(int x, int y)
{
    if(numOfRows%2 == 0)
    {
        for(int i = 0; i < 2; i++)
        {
            for(int j = 0; j < 2; j++)
            {
                if((numOfRows/2 - 1 + i) == x && (numOfRows/2 - 1 + j) == y)
                {
                    // Current position inside the middle
                    continue;
                }
                else
                {
                    int wallMask = (Direction::NORTH | Direction::EAST | Direction::SOUTH | Direction::WEST);
                    cells[numOfRows/2 - 1 + i][numOfRows/2 - 1 + j].setWallMask(wallMask);
                }
            }
        }
    }
}

void Maze::flowMaze(int x, int y, int stepCount)
{   
    if( cells[x][y].getValue() <= stepCount)
    {
        return;
    }
    cells[x][y].setValue(stepCount);

    if( cells[x][y].isAccessible(Direction::NORTH) && isValidPos(x+1) 
            && cells[x+1][y].getValue() > stepCount+1 )
    {
        flowMaze(x+1,y,stepCount+1);
    }
    if( cells[x][y].isAccessible(Direction::EAST) && isValidPos(y+1) 
            && cells[x][y+1].getValue() > stepCount+1 )
    {
        flowMaze(x,y+1,stepCount+1);
    }
    if( cells[x][y].isAccessible(Direction::SOUTH) && isValidPos(x-1) 
            && cells[x-1][y].getValue() > stepCount+1 )
    {
        flowMaze(x-1,y,stepCount+1);
    }
    if( cells[x][y].isAccessible(Direction::WEST) && isValidPos(y-1) 
            && cells[x][y-1].getValue() > stepCount+1 )
    {
        flowMaze(x,y-1,stepCount+1);
    }
}

int Maze::getWeightOfCell(int x, int y)
{
    return cells[x][y].getValue();
}