#include "game.hpp"

bool Game::DFS(int *mapT, int curx, int cury, int &count, const int &lastN) //用dfs判断加了障碍物后，map是不是有哈密顿路
{
    mapT[mapSize * curx + cury] = 1;
    ++count;
    if (count == lastN)
    {
        if ((!curx && cury == 1) || (curx == 1 && !cury))
            return true;
        else
        {
            mapT[mapSize * curx + cury] = 0;
            --count;
            return false;
        }
    }
    if (curx && !mapT[mapSize * (curx - 1) + cury])
    {
        if (DFS(mapT, curx - 1, cury, count, lastN))
            return true;
    }
    if (cury && !mapT[mapSize * curx + cury - 1])
    {
        if (DFS(mapT, curx, cury - 1, count, lastN))
            return true;
    }
    if (curx < mapSize - 1 && !mapT[mapSize * (curx + 1) + cury])
    {
        if (DFS(mapT, curx + 1, cury, count, lastN))
            return true;
    }
    if (cury < mapSize - 1 && !mapT[mapSize * curx + cury + 1])
    {
        if (DFS(mapT, curx, cury + 1, count, lastN))
            return true;
    }
    --count;
    mapT[mapSize * curx + cury] = 0;
    return false;
}

bool Game::HamiltonianLoop()
{
    int mapT[mapSize * mapSize];
    for (int i = 0; i < mapSize; ++i)
        for (int j = 0; j < mapSize; ++j)
            mapT[mapSize * i + j] = 0;
    for (int i = 0; i < obstacleN; ++i)
    {
        int x, y;
        obstacle[i]->getLocation(x, y);
        mapT[mapSize * x + y] = 1;
    }
    int useless = 0;
    return DFS(mapT, 0, 0, useless, mapSize * mapSize - obstacleN);
}
