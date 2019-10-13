#include "snake.hpp"
#include <fstream>
using namespace std;

Snake::Snake(int *&map, int mapSize, int &occupied, int i, int l) : length(1), snakeI(i), lineColor(Qt::white)//蛇长默认为map边长的一半
{
    int x, y, dx;    //蛇用一个单循环链表表示，链表元素为snakenode
    if (snakeI == 0) //链表的head是蛇的头，每一个元素指向它前面一个元素，head指向蛇的尾巴
    {
        direction = 3;
        x = 1;
        y = 1;
        dx = 1;
        color = QColor(125, 0, 0);
        headColor = QColor(255, 0, 125); //蛇0是正义的红色
    }
    else
    {
        direction = 1;
        x = mapSize;
        y = mapSize;
        dx = -1;
        color = QColor(0, 125, 0);
        headColor = QColor(0, 255, 125); //蛇1是邪恶的绿色
    }
    head = new snakeNode(x, y);
    head->front = head;
    map[mapSize * (x - 1) + y - 1] = snakeI + 1;
    ++occupied;
    for (int i = 1; i < l; ++i)
    {
        x += dx;
        grow(x, y); //长大到l为止
        map[mapSize * (x - 1) + y - 1] = snakeI + 1;
        ++occupied;
    }
}

Snake::Snake(Snake &snake) : length(snake.length), direction(snake.getDirection()) //蛇的复制构造函数
{
    snake.resetP();
    head = new snakeNode(snake.getX(), snake.getY());
    snakeNode *tmp = head;
    snake.nextP();
    for (int i = 1; i < length; ++i)
    {
        head->front = new snakeNode(snake.getX(), snake.getY());
        snake.nextP();
        head = head->front;
    }
    head->front = tmp;
    head = tmp;
}

Snake::~Snake()
{
    snakeNode *t1 = head, *t2;
    for (int i = 0; i < length; ++i)
    {
        t2 = t1->front;
        delete t1;
        t1 = t2;
    }
}
void Snake::act(int mapSize, int *&map) //外部调用的public函数
{
    active = true;
    int X, Y;
    nextStep(X, Y);
    if (X < 1 || X > mapSize || Y < 1 || Y > mapSize) //出界
    {
        die(snakeI);
        return;
    }
    int tmp = map[mapSize * (X - 1) + Y - 1];
    if (tmp == 5) //障碍物
    {
        die(snakeI);
        return;
    }
    if (tmp > 10) //食物
    {
        grow(X, Y);
        map[mapSize * (X - 1) + Y - 1] = snakeI + 1;
        createFood(snakeI, tmp - 11);
        return;
    }
    snakeNode *p = head->front;
    if (tmp == 0 || (p->x == X && p->y == Y)) //空的
    {
        map[mapSize * (p->x - 1) + p->y - 1] = 0;
        move(X, Y);
        map[mapSize * (X - 1) + Y - 1] = snakeI + 1;
        return;
    }
    if (tmp == 2 - snakeI) //撞到别蛇
        emit knockOther(X, Y, snakeI);
    else
        die(snakeI);//撞自己
    return;
}

void Snake::changeDirection(int d)
{
    if(active)
    {
        int tmp = (d - direction + 4) % 4;
        if (tmp == 0 || tmp == 2)
            return;
        direction = d;
        active = false;
    }
}

void Snake::paintSnake(QWidget *p, int sqSize)
{
    QPainter painter(p);
    painter.setPen(QColor(255, 255, 255));
    painter.setBrush(color);
    painter.setPen(lineColor);
    snakeNode *node = head->front, *node1 = head, *node2 = node->front;
    for (int i = 1; i <= length; ++i)
    {
        int x[3] = {node->x, node1->x, node2->x};
        int y[3] = {node->y, node1->y, node2->y};
        for (int j = 1; j < 3; ++j)
        {
            if ((i == 1 && j == 1) || (i == length && j == 2))
                continue;
            if (i == length)
                painter.setBrush(headColor);
            if (x[j] > x[0])
                painter.drawRect(int(sqSize * (0.2 + x[0])), int(sqSize * (0.2 + y[0])), int(0.8 * sqSize), int(0.6 * sqSize));
            else if (x[j] < x[0])
                painter.drawRect(sqSize * x[0], int(sqSize * (0.2 + y[0])), int(0.8 * sqSize), int(0.6 * sqSize));
            else if (y[j] > y[0])
                painter.drawRect(int(sqSize * (0.2 + x[0])), int(sqSize * (0.2 + y[0])), int(0.6 * sqSize), int(0.8 * sqSize));
            else
                painter.drawRect(int(sqSize * (0.2 + x[0])), int(sqSize * (y[0])), int(0.6 * sqSize), int(0.8 * sqSize));
        }
        node = node->front;
        node1 = node1->front;
        node2 = node2->front;
    }
}

void Snake::reverse() //逆置单链表
{
    snakeNode *t1 = head, *t2 = head->front, *t3 = head->front->front;
    head = t2;
    t2->front = t1;
    int x1 = t3->x, x2 = t2->x, y1 = t3->y, y2 = t2->y;
    if (x1 < x2)
        direction = 3;
    else if (x1 > x2)
        direction = 1;
    else if (y1 < y2)
        direction = 0;
    else
        direction = 2;
    for (int i = 1; i < length; ++i)
    {
        t1 = t2;
        t2 = t3;
        t3 = t3->front;
        t2->front = t1;
    }
}

void Snake::aiTestMove(int *&map, int mapSize, int foodx, int foody) //专用于ai移动的
{
    int X, Y;
    nextStep(X, Y);
    map[mapSize * (X - 1) + Y - 1] = 1;
    if (X == foodx && Y == foody)
        grow(X, Y);
    else
    {
        move(X, Y);
        snakeNode *node = head->front;
        X = node->x;
        Y = node->y;
        map[mapSize * (X - 1) + Y - 1] = 0;
    }
    return;
}

void Snake::nextStep(int &X, int &Y) const
{
    switch (direction)
    {
    case 0:
        X = head->x;
        Y = head->y + 1; //down
        break;
    case 1:
        X = head->x - 1;
        Y = head->y; //left
        break;
    case 2:
        X = head->x;
        Y = head->y - 1; //up
        break;
    case 3:
        X = head->x + 1;
        Y = head->y; //right
        break;
    }
}

void Snake::move(int X, int Y)
{
    head->front->x = X;
    head->front->y = Y;
    head = head->front;
}

void Snake::grow(int X, int Y)
{
    head->front = new snakeNode(X, Y, head->front);
    head = head->front;
    ++length;
}

void Snake::beBlack() //下面两个是致盲果实用的
{
    lineColor = Qt::black;
    headColor = color = Qt::black;
}

void Snake::recoverColor()
{
    lineColor = Qt::white;
    if (snakeI == 0)
    {
        color = QColor(125, 0, 0);
        headColor = QColor(255, 0, 125);
    }
    else
    {
        color = QColor(0, 125, 0);
        headColor = QColor(0, 255, 125);
    }
}
