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

    //проверка
    GameServer server;
    server.start(4242);

    GameClient client;
    client.connectToServer("127.0.0.1", 4242);


    QTimer::singleShot(1000, [&client]() {
        client.sendJoinRequest("Player1");
    });

    return a.exec();
}
