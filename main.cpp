#include "gamemenu.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GameMenu *w = new GameMenu;
    w->show();

    return a.exec();
}
