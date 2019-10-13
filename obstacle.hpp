#pragma once

#include <QWidget>
#include <QPainter>

class Obstacle : public QObject
{
    Q_OBJECT
public:
    Obstacle(int *&map, int mapSize, int &occupied);
    ~Obstacle() {}
    void printObstacle(QWidget *p, int sqSize);
    void getLocation(int &x,int &y){x=this->x;y=this->y;}

private:
    int x, y;
    QColor color;
};
