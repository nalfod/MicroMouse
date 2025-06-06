#include "maze.h"
#include <iostream>
#include "constants.h"
#include <cmath>
#include <iterator>

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
    updateCellWallMask(0,0,14);
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
    cells[x][y].isVisited(true);
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
        (cells[currx+1][curry].getWeight() < c.getWeight())) 
    {
        return CONSTS::Direction::NORTH;
    }
    
    if( c.isAccessible(CONSTS::Direction::EAST) && isValidPos(curry+1) &&
        (cells[currx][curry+1].getWeight() < c.getWeight())) 
    {
        return CONSTS::Direction::EAST;
    }

    if( c.isAccessible(CONSTS::Direction::SOUTH) && isValidPos(currx-1) &&
        (cells[currx-1][curry].getWeight() < c.getWeight())) 
    {
        return CONSTS::Direction::SOUTH;
    }

    if( c.isAccessible(CONSTS::Direction::WEST) && isValidPos(curry-1) &&
        (cells[currx][curry-1].getWeight() < c.getWeight())) 
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
        if(cells[currCellP->getX()+1][currCellP->getY()].getWeight() < lowestNeighbourValue) 
        {
            lowestNeighbourValue = cells[currCellP->getX()+1][currCellP->getY()].getWeight();
        }
        cN = &cells[currCellP->getX()+1][currCellP->getY()];
    }
    if( currCellP->isAccessible(CONSTS::Direction::EAST) && isValidPos(currCellP->getY()+1)) 
    {
        if(cells[currCellP->getX()][currCellP->getY()+1].getWeight() < lowestNeighbourValue) 
        {
            lowestNeighbourValue = cells[currCellP->getX()][currCellP->getY()+1].getWeight();
        }
        cE = &cells[currCellP->getX()][currCellP->getY()+1];
    }
    if( currCellP->isAccessible(CONSTS::Direction::SOUTH) && isValidPos(currCellP->getX()-1)) 
    {
        if(cells[currCellP->getX()-1][currCellP->getY()].getWeight() < lowestNeighbourValue) 
        {
            lowestNeighbourValue = cells[currCellP->getX()-1][currCellP->getY()].getWeight();
        }
        cS = &cells[currCellP->getX()-1][currCellP->getY()];
    }
    if( currCellP->isAccessible(CONSTS::Direction::WEST) && isValidPos(currCellP->getY()-1)) 
    {
        if(cells[currCellP->getX()][currCellP->getY()-1].getWeight() < lowestNeighbourValue) 
        {
            lowestNeighbourValue = cells[currCellP->getX()][currCellP->getY()-1].getWeight();
        }
        cW = &cells[currCellP->getX()][currCellP->getY()-1];
    }
    
    if(currCellP->getWeight() <= lowestNeighbourValue) 
    {
        currCellP->setWeight(lowestNeighbourValue+1);

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
            cells[i][j].setWeight(20000);
        }
    }
}

void Maze::reCalcMaze(bool toMid)
{
    resetValues();
    int posx = 0;
    int posy = 0;
    if(toMid)
    {
        if(numOfRows%2 != 0)
        {
            posx = numOfRows/2;
            posy = numOfRows/2;
        }
        else
        {
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
        }
    }
    //floodMaze(posx,posy,0,0);
    Cell* currCellP = &cells[posx][posy];
    updatelist.push_back(std::make_pair(currCellP,0));
    while(!updatelist.empty()) {
        floodMazeList();
    }
}

void Maze::floodMazeList()
{
    std::pair<Cell*,int>& currCell = updatelist.front();

    if(currCell.first->getWeight() <= currCell.second)
    {
        updatelist.pop_front();
        return;
    }
    currCell.first->setWeight(currCell.second);

    int x = currCell.first->getX();
    int y = currCell.first->getY();
    if( currCell.first->isAccessible(CONSTS::Direction::NORTH) && isValidPos(x+1) 
            && cells[x+1][y].getWeight() > currCell.second+1 )
    {
        auto it = updatelist.begin();
        bool insertIt = false;
        while(it != updatelist.end())
        {
            if((*it).first->getX() == (x+1) &&
                (*it).first->getY() == y)
                {
                    if((*it).second <= currCell.second+1)
                    {
                        break;
                    }
                    else
                    {
                        it = updatelist.erase(it);
                        insertIt = true;
                        break;
                    }
                }
                else
                {
                    it++;
                }
        }
        if(it == updatelist.end() || insertIt)
        {
            updatelist.push_back(std::make_pair(&cells[x+1][y],currCell.second+1));
        }
    }

    if( currCell.first->isAccessible(CONSTS::Direction::EAST) && isValidPos(y+1) 
            && cells[x][y+1].getWeight() > currCell.second+1 )
    {
        auto it = updatelist.begin();
        bool insertIt = false;
        while(it != updatelist.end())
        {
            if((*it).first->getX() == (x) &&
                (*it).first->getY() == y+1)
                {
                    if((*it).second <= currCell.second+1)
                    {
                        break;
                    }
                    else
                    {
                        it = updatelist.erase(it);
                        insertIt = true;
                        break;
                    }
                }
                else
                {
                    it++;
                }
        }
        if(it == updatelist.end() || insertIt)
        {
            updatelist.push_back(std::make_pair(&cells[x][y+1],currCell.second+1));
        }
    }

    if( currCell.first->isAccessible(CONSTS::Direction::SOUTH) && isValidPos(x-1) 
            && cells[x-1][y].getWeight() > currCell.second+1 )
    {
        auto it = updatelist.begin();
        bool insertIt = false;
        while(it != updatelist.end())
        {
            if((*it).first->getX() == (x-1) &&
                (*it).first->getY() == y)
                {
                    if((*it).second <= currCell.second+1)
                    {
                        break;
                    }
                    else
                    {
                        it = updatelist.erase(it);
                        insertIt = true;
                        break;
                    }
                }
                else
                {
                    it++;
                }
        }
        if(it == updatelist.end() || insertIt)
        {
            updatelist.push_back(std::make_pair(&cells[x-1][y],currCell.second+1));
        }
    }

    if( currCell.first->isAccessible(CONSTS::Direction::WEST) && isValidPos(y-1) 
            && cells[x][y-1].getWeight() > currCell.second+1 )
    {
        auto it = updatelist.begin();
        bool insertIt = false;
        while(it != updatelist.end())
        {
            if((*it).first->getX() == (x) &&
                (*it).first->getY() == y-1)
                {
                    if((*it).second <= currCell.second+1)
                    {
                        break;
                    }
                    else
                    {
                        it = updatelist.erase(it);
                        insertIt = true;
                        break;
                    }
                }
                else
                {
                    it++;
                }
        }
        if(it == updatelist.end() || insertIt)
        {
            updatelist.push_back(std::make_pair(&cells[x][y-1],currCell.second+1));
        }
    }

    updatelist.pop_front();
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
                    updateCellWallMask(numOfRows/2 - 1 + i, numOfRows/2 - 1 + j, wallMask);
                }
            }
        }
    }
}

void Maze::floodMaze(int x, int y, int stepCount)
{
    if( cells[x][y].getWeight() <= stepCount)
    {
        return;
    }
    cells[x][y].setWeight(stepCount);

    if( cells[x][y].isAccessible(CONSTS::Direction::NORTH) && isValidPos(x+1) 
            && cells[x+1][y].getWeight() > stepCount+1 )
    {
        floodMaze(x+1,y,stepCount+1);
    }
    if( cells[x][y].isAccessible(CONSTS::Direction::EAST) && isValidPos(y+1) 
            && cells[x][y+1].getWeight() > stepCount+1 )
    {
        floodMaze(x,y+1,stepCount+1);
    }
    if( cells[x][y].isAccessible(CONSTS::Direction::SOUTH) && isValidPos(x-1) 
            && cells[x-1][y].getWeight() > stepCount+1 )
    {
        floodMaze(x-1,y,stepCount+1);
    }
    if( cells[x][y].isAccessible(CONSTS::Direction::WEST) && isValidPos(y-1) 
            && cells[x][y-1].getWeight() > stepCount+1 )
    {
        floodMaze(x,y-1,stepCount+1);
    }
}

float Maze::getWeightOfCell(int x, int y)
{
    return cells[x][y].getWeight();
}

void Maze::closeUnknownCells()
{
    for(int i = 0; i < numOfRows; i++)
    {
        for(int j = 0; j < numOfRows; j++) 
        {
            if(!cells[i][j].getWasCellVisited())
            {
                int wallMask = (CONSTS::Direction::NORTH | CONSTS::Direction::EAST | CONSTS::Direction::SOUTH | CONSTS::Direction::WEST);
                updateCellWallMask(i, j, wallMask);
            }
        }
    }
}

std::string Maze::getShortestRoute2(int x, int y)
{
    std::vector<std::string> routes;
    std::string baseRoute = "";
    findShortestRoute2(x, y, baseRoute, routes);
    int minRotates = 20000;
    std::string shortestRoute;
    for(int i = 0; i < routes.size();i++)
    {
        std::string route = routes[i];
        int rotates = 0;
        for(int j = 1; j < route.size();j++)
        {
            if(route[j] != route[j-1])
            {
                rotates++;
            }
        }
        if(rotates < minRotates)
        {
            minRotates = rotates;
            shortestRoute = route;
        }
    }
    return shortestRoute;
}

void Maze::findShortestRoute2(int x, int y, std::string route, std::vector<std::string>& routes)
{
    Cell* currentCell = &cells[x][y];

    if(currentCell->getWeight() == 0)
    {
        routes.push_back(route);
        return;
    }
    if(currentCell->isAccessible(CONSTS::Direction::NORTH) && isValidPos(x+1)
        && currentCell->getWeight() > cells[x+1][y].getWeight())
    {
        std::string newRoute = (route + 'N');
        findShortestRoute2(x+1,y,newRoute,routes);
    }
    if(currentCell->isAccessible(CONSTS::Direction::EAST) && isValidPos(y+1)
        && currentCell->getWeight() > cells[x][y+1].getWeight())
    {
        std::string newRoute = (route + 'E');
        findShortestRoute2(x,y+1,newRoute,routes);
    }
    if(currentCell->isAccessible(CONSTS::Direction::SOUTH) && isValidPos(x-1)
        && currentCell->getWeight() > cells[x-1][y].getWeight())
    {
        std::string newRoute = (route + 'S');
        findShortestRoute2(x-1,y,newRoute,routes);
    }
    if(currentCell->isAccessible(CONSTS::Direction::WEST) && isValidPos(y-1)
        && currentCell->getWeight() > cells[x][y-1].getWeight())
    {
        std::string newRoute = (route + 'W');
        findShortestRoute2(x,y-1,newRoute,routes);
    }
}

std::string Maze::findShortestRoute(int x, int y)
{
    std::string route = "";

    Cell* currentCell = &cells[x][y];
    while(currentCell->getWeight() != 0)
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


void Maze::calcForSpeedRun(bool toMid)
{
    resetValues();
    int posx = 0;
    int posy = 0;
    if(toMid)
    {
        if(numOfRows%2 != 0)
        {
            posx = numOfRows/2;
            posy = numOfRows/2;
        }
        else
        {
            for(int i = 0; i < 2; i++)
            {
                for(int j = 0; j < 2; j++)
                {
                    if ( cells[numOfRows/2 - 1 + i][numOfRows/2 - 1 + j].getWallMask() < 15) 
                    {
                        posx = numOfRows/2 - 1 + i;
                        posy = numOfRows/2 - 1 + j;
                    }
                }
            }
        }
    }
    CONSTS::Direction startDir = (CONSTS::Direction)(cells[posx][posy].getWallMask()^15);
    updatelistSpeedRun.push_back(speedRunCalcItem(cells[posx][posy],0,0,startDir));
    while(!updatelistSpeedRun.empty()) {
        floodMazeListSpeedrun();
    }
    //floodMazeSpeedRun(posx,posy,0,0,startDir);
}

void Maze::floodMazeListSpeedrun()
{
    speedRunCalcItem& currItem = updatelistSpeedRun.front();

    if(currItem.cellP->getWeight() <= currItem.weight)
    {
        updatelistSpeedRun.pop_front();
        return;
    }
    currItem.cellP->setWeight(currItem.weight);

    int x = currItem.cellP->getX();
    int y = currItem.cellP->getY();
    if( currItem.cellP->isAccessible(CONSTS::Direction::NORTH) && isValidPos(x+1) )
    {
        auto it = updatelistSpeedRun.begin();
        bool insertIt = false;
        CONSTS::Direction newDir = currItem.currentDir;
        float newWeight = currItem.weight;
        int newSameInRow = currItem.sameInRow;
        if(newDir!=CONSTS::Direction::NORTH)
        {
            newDir = CONSTS::Direction::NORTH;
            newWeight += 1.5f;
            newSameInRow = 1;
        }
        else
        {
            newSameInRow++;
            newWeight += std::pow(0.5,newSameInRow);
        }
        while(it != updatelistSpeedRun.end())
        {
            if((*it).cellP->getX() == (x+1) &&
                (*it).cellP->getY() == y)
                {
                    if((*it).weight <= newWeight)
                    {
                        break;
                    }
                    else
                    {
                        it = updatelistSpeedRun.erase(it);
                        insertIt = true;
                        break;
                    }
                }
                else
                {
                    it++;
                }
        }
        if(it == updatelistSpeedRun.end() || insertIt)
        {
            updatelistSpeedRun.push_back(speedRunCalcItem(cells[x+1][y],newWeight,newSameInRow,newDir));
        }
    }

    if( currItem.cellP->isAccessible(CONSTS::Direction::EAST) && isValidPos(y+1) )
    {
        auto it = updatelistSpeedRun.begin();
        bool insertIt = false;
        CONSTS::Direction newDir = currItem.currentDir;
        float newWeight = currItem.weight;
        int newSameInRow = currItem.sameInRow;
        if(newDir!=CONSTS::Direction::EAST)
        {
            newDir = CONSTS::Direction::EAST;
            newWeight += 1.5f;
            newSameInRow = 1;
        }
        else
        {
            newSameInRow++;
            newWeight += std::pow(0.5,newSameInRow);
        }
        while(it != updatelistSpeedRun.end())
        {
            if((*it).cellP->getX() == (x) &&
                (*it).cellP->getY() == y+1)
                {
                    if((*it).weight <= newWeight)
                    {
                        break;
                    }
                    else
                    {
                        it = updatelistSpeedRun.erase(it);
                        insertIt = true;
                        break;
                    }
                }
                else
                {
                    it++;
                }
        }
        if(it == updatelistSpeedRun.end() || insertIt)
        {
            updatelistSpeedRun.push_back(speedRunCalcItem(cells[x][y+1],newWeight,newSameInRow,newDir));
        }
    }

    if( currItem.cellP->isAccessible(CONSTS::Direction::SOUTH) && isValidPos(x-1) )
    {
        auto it = updatelistSpeedRun.begin();
        bool insertIt = false;
        CONSTS::Direction newDir = currItem.currentDir;
        float newWeight = currItem.weight;
        int newSameInRow = currItem.sameInRow;
        if(newDir!=CONSTS::Direction::SOUTH)
        {
            newDir = CONSTS::Direction::SOUTH;
            newWeight += 1.5f;
            newSameInRow = 1;
        }
        else
        {
            newSameInRow++;
            newWeight += std::pow(0.5,newSameInRow);
        }
        while(it != updatelistSpeedRun.end())
        {
            if((*it).cellP->getX() == (x-1) &&
                (*it).cellP->getY() == y)
                {
                    if((*it).weight <= newWeight)
                    {
                        break;
                    }
                    else
                    {
                        it = updatelistSpeedRun.erase(it);
                        insertIt = true;
                        break;
                    }
                }
                else
                {
                    it++;
                }
        }
        if(it == updatelistSpeedRun.end() || insertIt)
        {
            updatelistSpeedRun.push_back(speedRunCalcItem(cells[x-1][y],newWeight,newSameInRow,newDir));
        }
    }

    if( currItem.cellP->isAccessible(CONSTS::Direction::WEST) && isValidPos(y-1) )
    {
        auto it = updatelistSpeedRun.begin();
        bool insertIt = false;
        CONSTS::Direction newDir = currItem.currentDir;
        float newWeight = currItem.weight;
        int newSameInRow = currItem.sameInRow;
        if(newDir!=CONSTS::Direction::WEST)
        {
            newDir = CONSTS::Direction::WEST;
            newWeight += 1.5f;
            newSameInRow = 1;
        }
        else
        {
            newSameInRow++;
            newWeight += std::pow(0.5,newSameInRow);
        }
        while(it != updatelistSpeedRun.end())
        {
            if((*it).cellP->getX() == (x) &&
                (*it).cellP->getY() == y-1)
                {
                    if((*it).weight <= newWeight)
                    {
                        break;
                    }
                    else
                    {
                        it = updatelistSpeedRun.erase(it);
                        insertIt = true;
                        break;
                    }
                }
                else
                {
                    it++;
                }
        }
        if(it == updatelistSpeedRun.end() || insertIt)
        {
            updatelistSpeedRun.push_back(speedRunCalcItem(cells[x][y-1],newWeight,newSameInRow,newDir));
        }
    }

    updatelistSpeedRun.pop_front();
}


void Maze::floodDirectionSpeedRun(CONSTS::Direction checkDirection, int x, int y, CONSTS::Direction currentDir, int sameInRow, float weight)
{
    if( cells[x][y].isAccessible(checkDirection) )
    {
        switch (checkDirection)
        {
            case CONSTS::Direction::NORTH:
                x++;
                break;

            case CONSTS::Direction::EAST:
                y++;
                break;

            case CONSTS::Direction::SOUTH:
                x--;
                break;

            case CONSTS::Direction::WEST:
                y--;
                break;
            default:
                break;
        }
        if(isValidPos(x) && isValidPos(y)) 
        {
            CONSTS::Direction newDir = currentDir;
            float newWeight = weight;
            int newSameInRow = sameInRow;
            if(newDir!=checkDirection)
            {
                newDir = checkDirection;
                newWeight += 1.5f;
                newSameInRow = 1;
            }
            else
            {
                newSameInRow++;
                newWeight += std::pow(0.5,newSameInRow);
            }
            floodMazeSpeedRun(x,y,newWeight,newSameInRow,newDir);
        }
    }
}

void Maze::floodMazeSpeedRun(int x, int y, float weight, int sameInRow, CONSTS::Direction currentDir)
{   
    if( cells[x][y].getWeight() <= weight)
    {
        return;
    }
    cells[x][y].setWeight(weight);

    floodDirectionSpeedRun(CONSTS::Direction::NORTH, x, y, currentDir, sameInRow, weight);
    floodDirectionSpeedRun(CONSTS::Direction::EAST, x, y, currentDir, sameInRow, weight);
    floodDirectionSpeedRun(CONSTS::Direction::SOUTH, x, y, currentDir, sameInRow, weight);
    floodDirectionSpeedRun(CONSTS::Direction::WEST, x, y, currentDir, sameInRow, weight);
}