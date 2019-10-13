#include "setWindow.hpp"

SetWindow::SetWindow(int size, int sqSize, int fN, int oN,bool e, QWidget *parent) : QWidget(parent)
{
    setFixedSize(400, 400);
    layout = new QVBoxLayout(this);
    hLayout = new QHBoxLayout *[5];
    for (int i = 0; i < 5; ++i)
        hLayout[i] = new QHBoxLayout();
    label = new QLabel[5];
    setWindowTitle("Set");
    resize(200, 200);
    console = new QLineEdit *[4];
    for (int i = 0; i < 5; ++i)
        console[i] = new QLineEdit;
    button = new QPushButton;
    button->setText("确定");
    label[0].setText("size:");
    label[1].setText("square size:");
    label[2].setText("food number:");
    label[3].setText("obstacle number:");
    label[4].setText("can snake eat other?");
    console[0]->setText(intToQString(size));
    console[1]->setText(intToQString(sqSize));
    console[2]->setText(intToQString(fN));
    console[3]->setText(intToQString(oN));
    QString tmp=e?"true":"false";
    console[4]->setText(tmp);
    for (int i = 0; i < 5; ++i)
    {
        hLayout[i]->addWidget(&label[i], 0, Qt::AlignmentFlag::AlignLeft);
        hLayout[i]->addWidget(console[i], 0, Qt::AlignmentFlag::AlignRight);
        layout->addLayout(hLayout[i]);
    }
    layout->addWidget(button);
    setLayout(layout);
    connect(button, SIGNAL(clicked()), this, SLOT(queding()));
}

SetWindow::~SetWindow()
{
    for (int i = 0; i < 5; ++i)
    {
        delete console[i];
        delete hLayout[i];
    }
    delete[] hLayout;
    delete layout;
    delete[] console;
    delete[] label;
}

QString SetWindow::intToQString(int a)
{
    QString tmp;
    while (a != 0)
    {
        tmp.push_front(a % 10 + '0');
        a = a / 10;
    }
    return tmp;
}

void SetWindow::queding()
{
    int size = console[0]->text().toInt();
    int squSize = console[1]->text().toInt();
    int foodN = console[2]->text().toInt();
    int obstacleN = console[3]->text().toInt();
    bool e=false;
    if(console[4]->text()=="true")
        e=true;
    else if(console[4]->text()=="false")
        e=false;
    Setting(size, squSize, foodN, obstacleN,e);
}
