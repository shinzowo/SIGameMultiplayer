#include "mainwindow.h"

#include <QApplication>
#include "GameServer.h"
#include "GameClient.h"
#include <QTimer>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
