#include "obstacle.hpp"

Obstacle::Obstacle(int *&map, int mapSize, int &occupied)
{
    int t1 = mapSize * mapSize - occupied; //和果子一样的简单随机算法
    int t2 = rand() % (t1);
    int z = 0;
    for (int i = 0; i < t2 || i == 0; ++z)
        if (!map[z])
            ++i;
    --z;
    x = z / mapSize + 1;
    y = z % mapSize + 1;
    map[mapSize * (x - 1) + y - 1] = 5;
    ++occupied;

    color = QColor(0, 0, 0, 100);
}

void Obstacle::printObstacle(QWidget *p, int sqSize)
{
    QPainter painter(p);
    painter.setBrush(color);
    painter.drawRect(x * sqSize, y * sqSize, sqSize, sqSize);
}

