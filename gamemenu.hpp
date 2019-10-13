#pragma once

#include <QWidget>
#include <QPushButton>
#include "game.hpp"
#include "setWindow.hpp"
#include "scoreBoard.hpp"

class GameMenu : public QWidget
{
    Q_OBJECT

public:
    explicit GameMenu(QWidget *parent = nullptr);
    ~GameMenu();

private:
    QPushButton **button;
    Game *game;
    SetWindow *setwindow;
    ScoreBoard *scoreboard;
    int size, sqSize, fN, oN;
    bool edible;

    void startGame(int sN, int fN, int oN, int s, int sqS, int ai)
    {
        game = new Game(sN, fN, oN, s, sqS, ai,edible);
        game->show();
    }

private slots:
    void f1() { startGame(1, fN, oN, size, sqSize, false); }
    void f2() { startGame(2, fN, oN, size, sqSize, false); }
    void f3() { startGame(1, fN, oN, size, sqSize, true); }
    void f4() { startGame(2, fN, oN, size, sqSize, true); }
    void f5();
    void f6();
    void Setting(int, int, int, int,bool);
};
