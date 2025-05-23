#include "maze.h"
#include <iostream>
#include "constants.h"

#include "utils/logging.h"

Maze::Maze(int mazeSize)
{
    numOfRows = mazeSize;
    initMaze();
}

void Maze::initMaze() {
    for(int i = 0; i < numOfRows; i++) {
        std::vector<Cell> row;
        row.reserve(numOfRows);
        cells.push_back(row);
    }

    for(int i = 0; i < numOfRows; i++) {
        for(int j = 0; j < numOfRows; j++) {
            Cell newCell(i, j, CONSTS::Direction::UNKNOWN, calcBaseVal(i,j));
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
    cells[x][y].setWallMask(wall);
    updateNeighbourWalls(x,y);
}

void Maze::updateNeighbourWalls(int x, int y)
{
    if( !cells[x][y].isAccessible(CONSTS::Direction::NORTH) && isValidPos(x+1))
    {
        int newWallMask = (cells[x+1][y].getWallMask() | CONSTS::Direction::SOUTH);
        cells[x+1][y].setWallMask(newWallMask);
    }
    if( !cells[x][y].isAccessible(CONSTS::Direction::EAST) && isValidPos(y+1))
    {
        int newWallMask = (cells[x][y+1].getWallMask() | CONSTS::Direction::WEST);
        cells[x][y+1].setWallMask(newWallMask);
    }
    if( !cells[x][y].isAccessible(CONSTS::Direction::SOUTH) && isValidPos(x-1))
    {
        int newWallMask = (cells[x-1][y].getWallMask() | CONSTS::Direction::NORTH);
        cells[x-1][y].setWallMask(newWallMask);
    }
    if( !cells[x][y].isAccessible(CONSTS::Direction::WEST) && isValidPos(y-1))
    {
        int newWallMask = (cells[x][y-1].getWallMask() | CONSTS::Direction::EAST);
        cells[x][y-1].setWallMask(newWallMask);
    }
}

CONSTS::Direction Maze::simpleMove(int currx, int curry) {
    Cell& c = cells[currx][curry];

    if( c.isAccessible(CONSTS::Direction::NORTH) && isValidPos(currx+1) &&
        (cells[currx+1][curry].getValue() < c.getValue())) 
    {
        return CONSTS::Direction::NORTH;
    }
    
    if( c.isAccessible(CONSTS::Direction::EAST) && isValidPos(curry+1) &&
        (cells[currx][curry+1].getValue() < c.getValue())) 
    {
        return CONSTS::Direction::EAST;
    }

    if( c.isAccessible(CONSTS::Direction::SOUTH) && isValidPos(currx-1) &&
        (cells[currx-1][curry].getValue() < c.getValue())) 
    {
        return CONSTS::Direction::SOUTH;
    }

    if( c.isAccessible(CONSTS::Direction::WEST) && isValidPos(curry-1) &&
        (cells[currx][curry-1].getValue() < c.getValue())) 
    {
        return CONSTS::Direction::WEST;
    }

    return CONSTS::Direction::UNKNOWN;
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
    
    if( currCellP->isAccessible(CONSTS::Direction::NORTH) && isValidPos(currCellP->getX()+1)) 
    {
        if(cells[currCellP->getX()+1][currCellP->getY()].getValue() < lowestNeighbourValue) 
        {
            lowestNeighbourValue = cells[currCellP->getX()+1][currCellP->getY()].getValue();
        }
        cN = &cells[currCellP->getX()+1][currCellP->getY()];
    }
    if( currCellP->isAccessible(CONSTS::Direction::EAST) && isValidPos(currCellP->getY()+1)) 
    {
        if(cells[currCellP->getX()][currCellP->getY()+1].getValue() < lowestNeighbourValue) 
        {
            lowestNeighbourValue = cells[currCellP->getX()][currCellP->getY()+1].getValue();
        }
        cE = &cells[currCellP->getX()][currCellP->getY()+1];
    }
    if( currCellP->isAccessible(CONSTS::Direction::SOUTH) && isValidPos(currCellP->getX()-1)) 
    {
        if(cells[currCellP->getX()-1][currCellP->getY()].getValue() < lowestNeighbourValue) 
        {
            lowestNeighbourValue = cells[currCellP->getX()-1][currCellP->getY()].getValue();
        }
        cS = &cells[currCellP->getX()-1][currCellP->getY()];
    }
    if( currCellP->isAccessible(CONSTS::Direction::WEST) && isValidPos(currCellP->getY()-1)) 
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

bool Maze::isCellDirectionBlocked(int currx, int curry, CONSTS::Direction dir)
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
                    int wallMask = (CONSTS::Direction::NORTH | CONSTS::Direction::EAST | CONSTS::Direction::SOUTH | CONSTS::Direction::WEST);
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

    if( cells[x][y].isAccessible(CONSTS::Direction::NORTH) && isValidPos(x+1) 
            && cells[x+1][y].getValue() > stepCount+1 )
    {
        flowMaze(x+1,y,stepCount+1);
    }
    if( cells[x][y].isAccessible(CONSTS::Direction::EAST) && isValidPos(y+1) 
            && cells[x][y+1].getValue() > stepCount+1 )
    {
        flowMaze(x,y+1,stepCount+1);
    }
    if( cells[x][y].isAccessible(CONSTS::Direction::SOUTH) && isValidPos(x-1) 
            && cells[x-1][y].getValue() > stepCount+1 )
    {
        flowMaze(x-1,y,stepCount+1);
    }
    if( cells[x][y].isAccessible(CONSTS::Direction::WEST) && isValidPos(y-1) 
            && cells[x][y-1].getValue() > stepCount+1 )
    {
        flowMaze(x,y-1,stepCount+1);
    }
}

int Maze::getWeightOfCell(int x, int y)
{
    return cells[x][y].getValue();
}

std::string Maze::findShortestRoute(int x, int y)
{
    std::string route = "";

    int offSetX = 0;
    int offSetY = 0;
    Cell* currentCell = &cells[x][y];
    while(currentCell->getValue() != 0)
    {
        CONSTS::Direction toDirection = simpleMove(x,y);
        switch (toDirection)
        {
        case CONSTS::Direction::NORTH:
            x++;
            route += 'N';
            break;
        case CONSTS::Direction::EAST:
            y++;
            route += 'E';
        break;
        case CONSTS::Direction::SOUTH:
            x--;
            route += 'S';
            break;        
        case CONSTS::Direction::WEST:
            y--;
            route += 'W';
        break;
        default:
            break;
        }
        currentCell = &cells[x][y];
    }
    return route;
}
