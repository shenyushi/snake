#include "game.hpp" //一些ai蛇想过的函数被放到这里

struct listNode //这是a*算法使用的元素
{
    int x, y;         //x,y为格子的坐标
    int G, H;         //G为从起点走到当前格子的步数，H为当前格子到终点的曼哈顿距离
    int F;            //F=G+H 是这条路线的估计值
    listNode *parent; //记录了当前节点的上一个节点，用于回溯
    listNode() {}
    listNode(int xx, int yy, int g, int h, listNode *p = nullptr) : x(xx), y(yy), G(g), H(h), F(g + h), parent(p) {}
    bool operator<(const listNode &x) { return F < x.F; } //元素的比较key值为F
    bool operator>(const listNode &x) { return F > x.F; }
};

class priorityQueue //这是listNode为元素的优先队列
{
    friend class Game;

private:
    listNode *data;
    int maxn;
    int current;
    bool far;          //far为true时为最大堆，否则默认为最小堆
    void doubleSpace() //最大堆用于找尾巴的路径，最小堆用以找食物
    {
        listNode *tmp = data;
        data = new listNode[2 * (maxn - 1)];
        for (int i = 1; i < maxn; ++i)
            data[i] = tmp[i];
        delete[] tmp;
        maxn = 2 * (maxn - 1);
    }
    void percolateDown(int hole)
    {
        listNode tmp = data[hole];
        int child;
        if (!far)
        {
            while (hole * 2 <= current)
            {
                child = hole * 2;
                if (child + 1 <= current && data[child + 1] < data[child])
                    ++child;
                if (tmp > data[child])
                {
                    data[hole] = data[child];
                    hole = child;
                }
                else
                    break;
            }
        }
        else
        {
            while (hole * 2 <= current)
            {
                child = hole * 2;
                if (child + 1 <= current && data[child + 1] > data[child])
                    ++child;
                if (tmp < data[child])
                {
                    data[hole] = data[child];
                    hole = child;
                }
                else
                    break;
            }
        }
        data[hole] = tmp;
    }

public:
    priorityQueue(bool f = false, int n = 21) : maxn(n), far(f)
    {
        current = 0;
        data = new listNode[maxn];
    }
    ~priorityQueue() { delete[] data; }
    bool isEmpty() const { return current == 0; }
    void enQueue(const listNode &x)
    {
        if (current == maxn - 1)
            doubleSpace();
        int hole = ++current;
        if (!far)
        {
            for (; hole > 1 && data[hole / 2] > x; hole /= 2)
                data[hole] = data[hole / 2];
        }
        else
        {
            for (; hole > 1 && data[hole / 2] < x; hole /= 2)
                data[hole] = data[hole / 2];
        }
        data[hole] = x;
    }
    listNode deQueue()
    {
        listNode tmp = data[1];
        data[1] = data[current--];
        percolateDown(1);
        return tmp;
    }
    listNode getHead() const { return data[1]; }
    listNode *find(int x, int y)
    {
        for (int i = 1; i <= current; ++i)
        {
            if (data[i].x == x && data[i].y == y)
                return &data[i];
        }
        return nullptr;
    }
    void buildHeap()
    {
        for (int i = current / 2; i > 0; --i)
            percolateDown(i);
    }
};

void Game::aiSnake() //控制ai蛇方向的函数（该函数只用于只有一条ai蛇的情况）
{
    snake[0]->resetP();
    int endx = food[0]->getX(), endy = food[0]->getY(), nowx, nowy, tailx, taily;
    snake[0]->tailLocation(tailx, taily);
    map[mapSize * (tailx - 1) + taily - 1] = 0; //把蛇的尾部和果子设置为可以通过。
    map[mapSize * (endx - 1) + endy - 1] = 0;   //只能处理一个果子的情况。
    if (newFood)                                //调用函数时果子刚被创造出来
    {                                           //由于只有一个ai蛇，果子被创建后，局面就确定了，在此直接求导果子的路径
        newFood = false;
        top = directionN = 0;
        Snake test(*snake[0]); //创建一个test蛇，模拟
        bool canReach;
        int *mapT = new int[mapSize * mapSize];
        for (int i = 0; i < mapSize; ++i) //设置备用map，用于test蛇。1表示不可通过，0表示可以
            for (int j = 0; j < mapSize; ++j)
            {
                if (map[mapSize * i + j])
                    mapT[mapSize * i + j] = 1;
                else
                    mapT[mapSize * i + j] = 0;
            }
        while (true)
        {
            test.resetP();
            test.getLocation(nowx, nowy);
            if (nowx == endx && nowy == endy) //到达，退出
                break;
            int d1 = 100;
            canReach = findRoute(mapT, nowx, nowy, endx, endy, d1); //到不了果实，退出
            if (!canReach)
                break;
            aiDirection[directionN++] = d1; //在类的成员变量中，记录方向
            test.changeDirection(d1);       //test蛇的转向与移动
            test.makeActive();
            test.aiTestMove(mapT, mapSize, endx, endy);
        }
        if (canReach) //能到果子
        {
            int useless;
            test.resetP();
            test.getLocation(nowx, nowy);
            test.tailLocation(tailx, taily);
            if (!findRoute(mapT, nowx, nowy, tailx, taily, useless)) //检测吃掉果子后能否找到尾巴
                top = directionN;
        }
        delete[] mapT;
    }
    if (top != directionN)                             //找得到尾巴
        snake[0]->changeDirection(aiDirection[top++]); //按之前存好的方向走，之后每次调用这个数列
    else                                               //找不到尾巴
    {
        int d2 = 100;
        snake[0]->resetP();
        snake[0]->getLocation(nowx, nowy);
        snake[0]->tailLocation(tailx, taily);
        findRoute(map, nowx, nowy, tailx, taily, d2, true); //找尾巴并转向，用最远路径
        snake[0]->changeDirection(d2);
        newFood = true; //仍然没有把changeDirecton数列设置好
    }
    snake[0]->resetP();
    snake[0]->tailLocation(tailx, taily);
    map[mapSize * (tailx - 1) + taily - 1] = 1; //将之前的尾部格子和果子的map值恢复
    map[mapSize * (endx - 1) + endy - 1] = 11;
}

void Game::aiSnakePro() //这是后来写的用于人机对战的函数
{                       //由于人的加入，局面会变化，所以算一步走一步。同样只能对于一个果子
    bool canReach;
    int *mapT = new int[mapSize * mapSize]; //建立备用map
    for (int i = 0; i < mapSize; ++i)
        for (int j = 0; j < mapSize; ++j)
        {
            if (map[mapSize * i + j])
                mapT[mapSize * i + j] = 1;
            else
                mapT[mapSize * i + j] = 0;
        }
    snake[1]->resetP();
    int endx = food[0]->getX(), endy = food[0]->getY(), nowx, nowy, tailx, taily;
    snake[1]->tailLocation(tailx, taily);
    mapT[mapSize * (tailx - 1) + taily - 1] = 0;
    mapT[mapSize * (endx - 1) + endy - 1] = 0; //备用map的尾部格子和食物格子设置为0
    int xxx, yyy;
    if (score[1] > score[0])
    {
        if(!edible)
        {
            snake[0]->resetP(); //如果ai蛇的分数比人的高，则可以和人对撞
            snake[0]->getLocation(xxx, yyy);
            mapT[mapSize * (xxx - 1) + yyy - 1] = 0;
        }
        else
        {
            snake[0]->resetP();
            for(int i=0;i<snake[0]->getLength();++i)
            {
                int x,y;
                snake[0]->getLocation(x,y);
                mapT[mapSize*(x-1)+y-1]=0;
                snake[0]->nextP();
            }
        }
    }
    int nextDirection = 1000;
    bool first = true;
    Snake test(*snake[1]);
    while (true)
    {
        test.resetP();
        test.getLocation(nowx, nowy); //用test蛇模拟
        if (nowx == endx && nowy == endy)
            break;
        int d1 = 100;
        canReach = findRoute(mapT, nowx, nowy, endx, endy, d1); //判定能否到达食物
        if (!canReach)
            break;
        if (first)
        {
            nextDirection = d1; //保存第一个方向
            first = false;
        }
        test.changeDirection(d1);
        test.makeActive();
        test.aiTestMove(mapT, mapSize, endx, endy);
    }
    bool canReachTail = true;
    if (canReach)
    {
        int useless;
        test.resetP();
        test.getLocation(nowx, nowy); //可以到达食物，判定吃了食物后能否到达尾部
        test.tailLocation(tailx, taily);
        if (!findRoute(mapT, nowx, nowy, tailx, taily, useless))
            canReachTail = false;
    }
    delete[] mapT;

    if (canReachTail)
        snake[1]->changeDirection(nextDirection); //可以到达尾部，改变为第一个方向
    else
    {
        mapT = new int[mapSize * mapSize];
        for (int i = 0; i < mapSize; ++i)
            for (int j = 0; j < mapSize; ++j)
            {
                if (map[mapSize * i + j])
                    mapT[mapSize * i + j] = 1;
                else
                    mapT[mapSize * i + j] = 0;
            }
        mapT[mapSize * (xxx - 1) + yyy - 1] = 0;
        mapT[mapSize * (endx - 1) + endy - 1] = 0;
        snake[1]->getLocation(nowx, nowy);
        mapT[mapSize * (nowx - 1) + nowy - 1] = 0; //否则找尾巴，用最远路径
        if (findRoute(mapT, nowx, nowy, endx, endy, nextDirection, true))
            snake[1]->changeDirection(nextDirection);
    } //找不到尾巴，随便走等死
}

bool Game::findRoute(int *aMap, int x1, int y1, int x2, int y2, int &direction, bool far) //a*算法找路径
{                                                                                         //x1,y1是起点，x2,y2是终点,direction是下一个方向，far=true则按最远路径找，否则默认按最经路径
    listNode *closeList = new listNode[mapSize * mapSize];
    int length = 0;                                                     //closelist的长度
    priorityQueue openList(far);                                        //openlist为优先队列,closelist为数列
    openList.enQueue(listNode(x1, y1, 0, abs(x2 - x1) + abs(y2 - y1))); //将起点存入openlist
    aMap[mapSize * (x1 - 1) + y1 - 1] = 2;                              //设置已经在openlist的节点为2
    listNode tmp;
    while (!openList.isEmpty())
    {
        tmp = openList.deQueue(); //出队一个F最小的元素（如果far==true,则输出最大的F的元素）
        closeList[length++] = tmp;
        aMap[mapSize * (tmp.x - 1) + tmp.y - 1] = 3; //把出队的元素进到closelist，在map上设置为3
        if (tmp.H == 0)                              //到达了
        {
            listNode *q = &closeList[length - 1], *p = q->parent; //回到第一个结点
            while (p->parent != nullptr)
            {
                q = p;
                p = p->parent;
            }
            int dx = q->x - p->x, dy = q->y - p->y; //求出第一个方向,其余的扔掉
            if (dx == 1)
                direction = 3;
            else if (dx == -1)
                direction = 1;
            else if (dy == 1)
                direction = 0;
            else
                direction = 2;
            for (int i = 1; i < length; ++i) //把closemap和openlist中的全设置为0，map可重复利用
                aMap[mapSize * (closeList[i].x - 1) + closeList[i].y - 1] = 0;
            for (int i = 1; i <= openList.current; ++i)
                aMap[mapSize * (openList.data[i].x - 1) + openList.data[i].y - 1] = 0;
            aMap[mapSize * (x1 - 1) + y1 - 1] = 1; //起点设置为1
            delete[] closeList;
            return true;
        }
        if (tmp.x != 1) //如果不是第一列
        {
            int pp = mapSize * (tmp.x - 2) + tmp.y - 1; //左边一个
            int k = aMap[pp];
            switch (k)
            {
            case 1: //不可到达
            case 5: //障碍物，只在只有ai的条件下出现（后来的函数复制了备用map）
            case 3: //在closelist中
                break;
            case 0: //空的
                openList.enQueue(listNode(tmp.x - 1, tmp.y, tmp.G + 1, abs(x2 - tmp.x + 1) + abs(y2 - tmp.y), &closeList[length - 1]));
                aMap[pp] = 2; //进入openlist
                break;
            default: //在openlist中
                listNode *fix = openList.find(tmp.x - 1, tmp.y);
                if (!far && tmp.G + 1 < fix->G) //如果far==false，则判断当前的G是否比队中的更小
                {
                    fix->G = tmp.G + 1; //是的话队中的G改为当前G，重新建堆
                    fix->parent = &closeList[length - 1];
                    fix->F = fix->H + fix->G;
                    openList.buildHeap();
                }
                if (far && tmp.G + 1 > fix->G) //否则，判断当前G是否更大
                {
                    fix->G = tmp.G + 1;
                    fix->parent = &closeList[length - 1];
                    fix->F = fix->H + fix->G;
                    openList.buildHeap();
                }
            }
        }
        if (tmp.y != 1) //当前不是第一一行
        {
            int pp = mapSize * (tmp.x - 1) + tmp.y - 2;
            int k = aMap[pp];
            switch (k)
            {
            case 1:
            case 5:
            case 3:
                break;
            case 0:
                openList.enQueue(listNode(tmp.x, tmp.y - 1, tmp.G + 1, abs(x2 - tmp.x) + abs(y2 - tmp.y + 1), &closeList[length - 1]));
                aMap[pp] = 2;
                break;
            default:
                listNode *fix = openList.find(tmp.x, tmp.y - 1);
                if (!far && tmp.G + 1 < fix->G)
                {
                    fix->G = tmp.G + 1;
                    fix->parent = &closeList[length - 1];
                    fix->F = fix->H + fix->G;
                    openList.buildHeap();
                }
                if (far && tmp.G + 1 > fix->G)
                {
                    fix->G = tmp.G + 1;
                    fix->parent = &closeList[length - 1];
                    fix->F = fix->H + fix->G;
                    openList.buildHeap();
                }
            }
        }
        if (tmp.x != mapSize) //当前不是最后一列
        {
            int pp = mapSize * tmp.x + tmp.y - 1;
            int k = aMap[pp];
            switch (k)
            {
            case 1:
            case 5:
            case 3:
                break;
            case 0:
                openList.enQueue(listNode(tmp.x + 1, tmp.y, tmp.G + 1, abs(x2 - tmp.x - 1) + abs(y2 - tmp.y), &closeList[length - 1]));
                aMap[pp] = 2;
                break;
            default:
                listNode *fix = openList.find(tmp.x + 1, tmp.y);
                if (!far && tmp.G + 1 < fix->G)
                {
                    fix->G = tmp.G + 1;
                    fix->parent = &closeList[length - 1];
                    fix->F = fix->H + fix->G;
                    openList.buildHeap();
                }
                if (far && tmp.G + 1 > fix->G)
                {
                    fix->G = tmp.G + 1;
                    fix->parent = &closeList[length - 1];
                    fix->F = fix->H + fix->G;
                    openList.buildHeap();
                }
            }
        }
        if (tmp.y != mapSize) //当前不是最后一行
        {
            int pp = mapSize * (tmp.x - 1) + tmp.y;
            int k = aMap[pp];
            switch (k)
            {
            case 1:
            case 5:
            case 3:
                break;
            case 0:
                openList.enQueue(listNode(tmp.x, tmp.y + 1, tmp.G + 1, abs(x2 - tmp.x) + abs(y2 - tmp.y - 1), &closeList[length - 1]));
                aMap[pp] = 2;
                break;
            default:
                listNode *fix = openList.find(tmp.x, tmp.y + 1);
                if (!far && tmp.G + 1 < fix->G)
                {
                    fix->G = tmp.G + 1;
                    fix->parent = &closeList[length - 1];
                    fix->F = fix->H + fix->G;
                    openList.buildHeap();
                }
                if (far && tmp.G + 1 > fix->G)
                {
                    fix->G = tmp.G + 1;
                    fix->parent = &closeList[length - 1];
                    fix->F = fix->H + fix->G;
                    openList.buildHeap();
                }
            }
        }
    }
    delete[] closeList;
    return false;
}
