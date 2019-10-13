#include "gamemenu.hpp"
GameMenu::GameMenu(QWidget *) : game(nullptr)
{
    size = 10; //默认的参数，可以在set中修改
    sqSize = 30;
    fN = 1;
    oN = 4;
    edible=false;

    srand(time(nullptr));
    setWindowTitle("snake");
    resize(200, 290);

    button = new QPushButton *[6];
    for (int i = 0; i < 6; ++i)
        button[i] = new QPushButton(this);
    button[0]->setText("New Game");
    button[1]->setText("Multiplayer games");
    button[2]->setText("AI snake");
    button[3]->setText("Ai And Human");
    button[4]->setText("ScoreBoard");
    button[5]->setText("Set");
    button[0]->move(50, 30);
    button[1]->move(30, 70);
    button[2]->move(50, 110);
    button[3]->move(45, 150);
    button[4]->move(50, 190);
    button[5]->move(50, 230);
    connect(button[0], SIGNAL(clicked()), this, SLOT(f1()));
    connect(button[1], SIGNAL(clicked()), this, SLOT(f2()));
    connect(button[2], SIGNAL(clicked()), this, SLOT(f3()));
    connect(button[3], SIGNAL(clicked()), this, SLOT(f4()));
    connect(button[4], SIGNAL(clicked()), this, SLOT(f5()));
    connect(button[5], SIGNAL(clicked()), this, SLOT(f6()));
}

GameMenu::~GameMenu()
{
    for (int i = 0; i < 6; i++)
        delete button[i];
    delete[] button;
    if (game != nullptr)
        delete game;
}

void GameMenu::f5()
{
    scoreboard = new ScoreBoard();
    scoreboard->show();
}

void GameMenu::f6()
{
    setwindow = new SetWindow(size, sqSize, fN, oN,edible);
    setwindow->show();
    connect(setwindow, SIGNAL(Setting(int, int, int, int, bool)), this, SLOT(Setting(int, int, int, int, bool)));
}

void GameMenu::Setting(int size, int sqSize, int fN, int oN, bool e)
{
    this->size = size;
    this->sqSize = sqSize;
    this->fN = fN;
    this->oN = oN;
    edible=e;
}
