#include "game.hpp"

#include <fstream>
using namespace std;

Game::Game(int sN, int fN, int oN, int s, int sqS, bool ai,bool e)
    : mapSize(s), sqSize(sqS),
      snakeN(sN), foodN(fN), obstacleN(oN),
      snake(new Snake *[snakeN]), food(new Food *[foodN]), obstacle(new Obstacle *[obstacleN]),
      map(new int[mapSize * mapSize]), occupied(0),
      interval(new int[snakeN]), timer(new QTimer *[snakeN]), effectTimer(new QTimer *[snakeN]),
      board(new QLabel *[snakeN]), score(new int[snakeN]),
      dieWindow(nullptr), winWindow(nullptr), isBlind(false), blindWindow(nullptr),
      ai(ai), aiDirection(new int[200]), newFood(true), aiFoodI(0),edible(e)
{
    setWindowTitle("snake game");
    resize(int(1.25 * mapSize * sqSize + 100), int(1.25 * mapSize * sqSize));
    for (int i = 0; i < mapSize; i++)
        for (int j = 0; j < mapSize; j++)
            map[mapSize * i + j] = 0; //x:map[k]/10, y:map[k]%10
    for (int i = 0; i < snakeN; ++i)
    {
        snake[i] = new Snake(map, mapSize, occupied, i, mapSize / 2);
        interval[i] = 300; //默认的间隔
        if (ai && snakeN == 1)
            interval[i] /= 20; //ai的间隔
        timer[i] = new QTimer(this);
        timer[i]->start(interval[i]);
        effectTimer[i] = new QTimer(this);
        board[i] = new QLabel(this);
        board[i]->resize(100, 50);
        board[i]->move(int(sqSize * mapSize + 100), 50 * (i + 1));
        board[i]->setText(QString("Score%1:\n0").arg(i));
        board[i]->show();
        score[i] = 0;
        connect(snake[i], SIGNAL(die(int)), this, SLOT(die(int)));
        connect(snake[i], SIGNAL(createFood(int, int)), this, SLOT(createFood(int, int)));
        connect(snake[i], SIGNAL(knockOther(int, int, int)), this, SLOT(judge(int, int, int)));
    }

    for (int i = 0; i < obstacleN; ++i)
        obstacle[i] = new Obstacle(map, mapSize, occupied);
    if (mapSize < 7) //只在map大小小于7时判断哈密顿路，否则运行时间太长了，所以没有实用价值
        if (!HamiltonianLoop())
            setWindowTitle("snake game(not hamiltonion loop)");
    for (int i = 0; i < foodN; ++i)
    {
        bool t = ai && snakeN == 1;
        food[i] = new Food(map, mapSize, occupied, i, t);
        connect(food[i], SIGNAL(win()), this, SLOT(win()));
    }
    connect(timer[0], SIGNAL(timeout()), this, SLOT(f1()));
    if (sN == 2)
        connect(timer[1], SIGNAL(timeout()), this, SLOT(f2()));
    connect(effectTimer[0], SIGNAL(timeout()), this, SLOT(g1()));
    if (sN == 2)
        connect(effectTimer[1], SIGNAL(timeout()), this, SLOT(g2()));
    update();
}

Game::~Game()
{
    for (int i = 0; i < snakeN; ++i)
    {
        delete snake[i];
        delete timer[i];
        delete effectTimer[i];
        delete board[i];
    }
    delete[] snake;
    delete[] timer;
    delete[] effectTimer;
    delete[] board;
    delete[] score;
    for (int i = 0; i < foodN; ++i)
        delete food[i];
    delete[] food;
    for (int i = 0; i < obstacleN; ++i)
        delete obstacle[i];
    delete[] obstacle;
    delete[] map;
    delete[] interval;
    if (dieWindow != nullptr)
        delete dieWindow;
    if (winWindow != nullptr)
        delete dieWindow;
    delete[] aiDirection;
    delete blindWindow;
}

void Game::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_W:
        snake[0]->changeDirection(2);
        break;
    case Qt::Key_A:
        snake[0]->changeDirection(1);
        break;
    case Qt::Key_S:
        snake[0]->changeDirection(0);
        break;
    case Qt::Key_D:
        snake[0]->changeDirection(3);
        break;
    }
    if (snakeN > 1 && !ai) //玩家二用IJKL代替WASD
    {
        switch (event->key())
        {
        case Qt::Key_I:
            snake[1]->changeDirection(2);
            break;
        case Qt::Key_J:
            snake[1]->changeDirection(1);
            break;
        case Qt::Key_K:
            snake[1]->changeDirection(0);
            break;
        case Qt::Key_L:
            snake[1]->changeDirection(3);
            break;
        }
    }
}

void Game::paintEvent(QPaintEvent *)
{
    paintWall();
    for (int i = 0; i < snakeN; ++i)
        snake[i]->paintSnake(this, sqSize);
    for (int i = 0; i < foodN; ++i)
        food[i]->paintFood(this, sqSize);
    for (int i = 0; i < obstacleN; ++i)
        obstacle[i]->printObstacle(this, sqSize);
}

void Game::paintWall()
{
    QPainter painter(this);
    QColor wallColor(80, 80, 80);
    painter.setPen(wallColor);
    painter.setBrush(wallColor);
    painter.drawRect(0, 0, sqSize, (mapSize + 2) * sqSize);
    painter.drawRect(0, 0, (mapSize + 2) * sqSize, sqSize);
    painter.drawRect(0, (mapSize + 1) * sqSize, (mapSize + 2) * sqSize, sqSize);
    painter.drawRect((mapSize + 1) * sqSize, 0, sqSize, (mapSize + 2) * sqSize);
    for (int i = 2 * sqSize; i <= sqSize * mapSize; i += sqSize)
    {
        painter.drawLine(i, sqSize, i, (mapSize + 1) * sqSize);
        painter.drawLine(sqSize, i, (mapSize + 1) * sqSize, i);
    }
}

void Game::upDate(int sI)
{
    if (ai)
    {
        if (snakeN == 1)
            aiSnake();
        else
            aiSnakePro();
    }
    snake[sI]->act(mapSize, map);
    update();
    if (isBlind && blindWindow == nullptr) //blind状态就加一个黑色的label，并把蛇变成黑色
    {
        blindWindow = new QLabel(this);
        blindWindow->setGeometry(0, 0, 1000, 1000);
        QPalette pa = blindWindow->palette();
        pa.setBrush(QPalette::Background, QBrush(Qt::black));
        setPalette(pa);
        snake[sI]->beBlack();
        blindWindow->show();
        update();
    }
}

void Game::recover(int snakeI) //特效的恢复
{
    if (isBlind)
    {
        isBlind = false;
        QPalette pa = blindWindow->palette();
        pa.setBrush(QPalette::Background, QBrush(Qt::white));
        setPalette(pa);
        blindWindow->show();
        snake[snakeI]->recoverColor();
        update();
        delete blindWindow;
        blindWindow = nullptr;
    }
    else
    {
        effectTimer[snakeI]->stop();
        timer[snakeI]->start(interval[snakeI]);
    }
}

void Game::createFood(int snakeI, int foodI)
{
    newFood = true;
    bool flag = true;
    int tmp = food[foodI]->effect();
    switch (tmp)
    {
    case 1:
        snake[snakeI]->reverse();
        score[snakeI] += 20;
        break;
    case 2:
        flag = false;
        timer[snakeI]->start(100);
        effectTimer[snakeI]->start(700);
        score[snakeI] += 20;
        break;
    case 3:
        isBlind = true;
        effectTimer[snakeI]->start(1400);
        score[snakeI] += 20;
        break;
    default:
        score[snakeI] += 10;
        break;
    }
    board[snakeI]->setText(QString("Score%1:\n%2").arg(snakeI + 1).arg(score[snakeI]));
    board[snakeI]->show();
    delete food[foodI];
    food[foodI] = new Food(map, mapSize, occupied, foodI, ai);
    if (!ai)
        interval[snakeI] -= 1; //吃得越多，跑得越快
    if (flag)
        timer[snakeI]->start(interval[snakeI]);
}

#include <fstream>

void Game::die(int sI)
{
    for (int i = 0; i < snakeN; ++i)
    {
        delete timer[i];
        delete effectTimer[i];
    }
    dieWindow = new QLabel(this);
    dieWindow->move(70, 100);
    dieWindow->resize(250, 50);
    QPalette pa = dieWindow->palette();
    pa.setBrush(QPalette::Background, QBrush(Qt::red)); //把背景弄成红的
    setPalette(pa);
    setWindowOpacity(0.85);
    dieWindow->setFont(QFont("Times", 20, QFont::Bold));
    dieWindow->setText(QString("PLAYER%1 DIE!").arg(sI + 1));
    dieWindow->show();

    string str;
    char tmp;
    ifstream infile("./score.txt");
    while (infile >> tmp)
        str.push_back(tmp);
    int score1 = 0;
    for (int i = 0; i < str.length(); ++i)
        score1 = score1 * 10 + str[i] - '0';
    infile.close();
    if (score[sI] > score1)
    {
        ofstream outfile("./score.txt");
        if (outfile)
            outfile << score[sI];
        outfile.close();
    }
}

void Game::win()
{
    for (int i = 0; i < snakeN; ++i)
    {
        delete timer[i];
        delete effectTimer[i];
    }
    winWindow = new QLabel(this);
    winWindow->move(70, 100);
    winWindow->resize(250, 50);
    QPalette pa = winWindow->palette();
    pa.setBrush(QPalette::Background, QBrush(Qt::blue)); //背景是蓝色的
    setPalette(pa);
    setWindowOpacity(0.85);
    winWindow->setFont(QFont("Times", 20, QFont::Bold));
    winWindow->setText("YOU WIN!!!");
    winWindow->show();
}

void Game::judge(int X, int Y, int snakeI) //两条蛇头撞头，分数大的赢
{
    int other = 1 - snakeI;
    snake[other]->resetP();
    if(edible)
    {
        if (score[snakeI] > score[other])
            die(other);
        else if (score[snakeI] < score[other])
            die(snakeI);
        else
        {
            int x,y;
            snake[other]->getLocation(x,y);
            if(x==X && y==Y)
            {
                if (score[snakeI] > score[other])
                    die(other);
                else
                    die(snakeI);
                return;
            }
            die(snakeI);
        }
    }
    else
    {
        int x,y;
        snake[other]->getLocation(x,y);
        if(x==X && y==Y)
        {
            if (score[snakeI] > score[other])
                die(other);
            else
                die(snakeI);
            return;
        }
        die(snakeI);
    }
}
