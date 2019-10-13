#pragma once

#include <QLineEdit>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QString>

class SetWindow : public QWidget
{
    Q_OBJECT
private:
    QLineEdit **console;
    QVBoxLayout *layout;
    QHBoxLayout **hLayout;
    QLabel *label;
    QPushButton *button;

public:
    SetWindow(int size, int sqSize, int fN, int oN,bool e, QWidget *parent = NULL);
    ~SetWindow();

private:
    QString intToQString(int a);
private slots:
    void queding();
signals:
    void Setting(int, int, int, int,bool);
};
