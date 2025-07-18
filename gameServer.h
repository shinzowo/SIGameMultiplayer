#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include "GameLogic.h"

class GameServer : public QTcpServer {
    Q_OBJECT

public:
    explicit GameServer(QObject* parent = nullptr);
    void start(quint16 port);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void onClientReadyRead();
    void onClientDisconnected();

private:
    GameLogic m_logic;
    QMap<QTcpSocket*, QString> m_clients;

    void broadcastMessage(const QString& message);
    void handleMessage(QTcpSocket* client, const QString& message);
};

#endif // GAMESERVER_H
