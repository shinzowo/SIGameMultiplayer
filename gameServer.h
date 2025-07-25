#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>



class GameServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit GameServer(QObject *parent = nullptr);
    bool startServer(quint16 port);
    QString hostNickname;

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onClientDisconnected();
    void onClientReadyRead();

private:
    QMap<QTcpSocket*, QString> clients;
    QMap<QString, bool> readyPlayers;
    void broadcastLobby();
    void checkAllReady();
    const QString READY_STATUS = "ready_status";
    const QString GAME_START = "game_start";
};



