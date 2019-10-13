#pragma once
#include <QWidget>
#include <QPainter>

enum foodKind //这是一个食物类
{
    NORM = 0,
    FAST,
    REVE,
    BLIND
};

class Food : public QWidget
{
    Q_OBJECT
public:
    Food(int *&map, int mapSize, int &occupied, int i, bool ai);
    ~Food() {}
    void paintFood(QWidget *p, int sqSize);
    int getX() const { return x; }
    int getY() const { return y; }
    int effect();

private:
    int x, y;
    foodKind kind;
    QColor color;
    int foodI;
    bool ai;
signals:
    void win();
};
