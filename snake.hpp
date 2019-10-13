
#pragma once

#include <QWidget>
#include <QPainter>

class Snake : public QObject
{
    Q_OBJECT

public:
    Snake() {}
    Snake(int *&map, int mapSize, int &occupied, int i, int l = 5);
    Snake(Snake &snake);
    ~Snake();
    void act(int mapSize, int *&map);
    void changeDirection(int d);
    void paintSnake(QWidget *p, int sqSize);
    void reverse();
    void aiTestMove(int *&map, int mapSize, int foodx, int foody);

private:
    struct snakeNode
    {
        int x, y;
        snakeNode *front;
        snakeNode(int xx = -1, int yy = -1, snakeNode *f = nullptr) : x(xx), y(yy), front(f) {}
    };
    snakeNode *head;
    int length;
    int direction; //0:down 1:left 2:up 3:right
    bool active;
    int snakeI;
    QColor color, headColor, lineColor;
    void nextStep(int &X, int &Y) const;
    void move(int X, int Y);
    void grow(int X, int Y);
signals:
    void die(int);
    void createFood(int, int);

private:
    snakeNode *p;

public:
    void resetP() { p = head; }
    void nextP() { p = p->front; }
    void getLocation(int &X, int &Y) const
    {
        X = p->x;
        Y = p->y;
    }
    int getX() { return p->x; }
    int getY() { return p->y; }
    int getLength() const { return length; }
    int getDirection() const { return direction; }
    void tailLocation(int &X, int &Y) const
    {
        snakeNode *p = head->front;
        X = p->x;
        Y = p->y;
    }
    void beBlack();
    void recoverColor();
    void makeActive(){active=true;}
signals:
    void knockOther(int,int,int);
};
