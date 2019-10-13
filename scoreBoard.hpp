#ifndef SCOREBOARD_HPP
#define SCOREBOARD_HPP

#include <QWidget>
#include <QLabel>
#include <fstream>
#include <QDebug>
#include <string>
using namespace std;

class ScoreBoard : public QWidget
{
    Q_OBJECT
public:
    ScoreBoard(QWidget *parent = NULL) : QWidget(parent)
    {
        resize(300, 300);
        label = new QLabel(this);
        label->move(10, 10);
        QString score;

        char tmp;
        string str;
        fstream infile("./score.txt");
        if (!infile)
        {
            qDebug() << "error";
            return;
        }
        while (infile >> tmp)
            str.push_back(tmp);

        score = QString::fromStdString(str);
        QString information = "You best score is:" + score;
        label->setText(information);
    }
    ~ScoreBoard()
    {
        delete label;
    }

private:
    QLabel *label;
};

#endif // SCOREBOARD_HPP
