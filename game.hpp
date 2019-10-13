#pragma once
#include <QWidget>
#include "snake.hpp"
#include "food.hpp"
#include "obstacle.hpp"
#include <QTimer>
#include <QLabel>
#include <QPainter>
#include <QKeyEvent>
#include <QFont>

struct listNode;

class Game : public QWidget
{
    Q_OBJECT
public:
    Game(int sN, int fN, int oN, int s, int sqS, bool ai,bool e);
    ~Game();

protected:
    void keyPressEvent(QKeyEvent *);
    void paintEvent(QPaintEvent *);

private:
    int mapSize, sqSize;
    int snakeN, foodN, obstacleN;
    Snake **snake;
    Food **food;
    Obstacle **obstacle;
    int *map, occupied; //map 0:nothing, 1:snake1, 2:snake2, 5:obstacle, 11:food1, 12:food2 ....
    int *interval;
    QTimer **timer;
    QTimer **effectTimer;
    QLabel **board;
    int *score;
    QLabel *dieWindow, *winWindow;
    void paintWall();
    void upDate(int sI);
    void recover(int sI);
    bool isBlind;
    QLabel *blindWindow;

    bool ai;
    int *aiDirection;
    int directionN,top;
    bool newFood;
    bool edible;

    void aiSnake();
    bool findRoute(int *aMap, int x1, int y1, int x2, int y2, int &direction, bool far = false);
    void aiSnakePro();
    int aiFoodI;
    bool HamiltonianLoop();
    bool DFS(int *mapT,int curx,int cury,int &count,const int &lastN);

private slots:
    void die(int sI);
    void win();
    void createFood(int foodI, int snakeI);
    void f1() { upDate(0); }
    void f2() { upDate(1); }
    void g1() { recover(0); }
    void g2() { recover(1); }
    void judge(int X,int Y,int snakeI);
};
