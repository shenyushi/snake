#include "food.hpp"

Food::Food(int *&map, int mapSize, int &occupied, int i, bool ai) : foodI(i), ai(ai)
{
    if (occupied == mapSize * mapSize) //随机果子的算法
    {                                  //算出没有被占用的格子数t1，取随机的t1的余数t2
        win();                         //从头数过来t2+1个就是果实位置
        return;
    }
    int t1 = mapSize * mapSize - occupied;

    int t2 = rand() % (t1);
    int z = 0;
    for (int i = 0; i < t2 || i == 0; ++z)
        if (!map[z])
            ++i;
    --z;
    x = z / mapSize + 1;
    y = z % mapSize + 1;
    map[mapSize * (x - 1) + y - 1] = 11 + foodI;
    ++occupied;
    int tmp = rand() % 10;
    if (ai)
        tmp =rand() % 7;
    if (tmp < 5)
    {
        kind = NORM;
        color = QColor(255, 0, 0); //红色的普通果实，10分
    }
    else if (tmp < 7)
    {
        kind = REVE;
        color = QColor(125, 125, 0); //以下为特效果实，20分。黄色的颠倒果实
    }
    else if (tmp < 9)
    {
        kind = FAST;
        color = QColor(0, 125, 125); //蓝绿色的加速果实，持续1s
    }
    else
    {
        kind = BLIND;
        color = QColor(125, 0, 125); //紫色的致盲果实，持续2s
    }
}

void Food::paintFood(QWidget *p, int sqSize)
{
    QPainter painter(p);
    painter.setBrush(color);
    painter.drawEllipse(x * sqSize, y * sqSize, sqSize, sqSize);
}

int Food::effect()
{
    switch (kind)
    {
    case REVE:
        return 1;
    case FAST:
        return 2;
    case BLIND:
        return 3;
    default:
        return 0;
    }
}
