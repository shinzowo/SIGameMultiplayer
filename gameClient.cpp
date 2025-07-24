#include "GameClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

GameClient::GameClient(QObject *parent)
    : QObject(parent), socket(new QTcpSocket(this))
{
    connect(socket, &QTcpSocket::connected, this, &GameClient::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &GameClient::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &GameClient::onError);
    connect(socket, &QTcpSocket::disconnected, this, []() {
        qDebug() << "Disconnected from server";
    });
}

GameClient::~GameClient()
{
    socket->disconnectFromHost();
    socket->deleteLater();
}

void GameClient::connectToServer(const QString &host, quint16 port, const QString &nickname)
{
    this->nickname = nickname;
    socket->connectToHost(host, port);
}

void GameClient::onConnected()
{
    QJsonObject obj;
    obj["type"] = "join";
    obj["nickname"] = nickname;

    QJsonDocument doc(obj);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");

    qDebug() << "Connected and sent nickname:" << nickname;
}

void GameClient::onReadyRead()
{
    buffer += socket->readAll();

    while (true) {
        int newlineIndex = buffer.indexOf('\n');
        if (newlineIndex == -1)
            break;

        QByteArray line = buffer.left(newlineIndex);
        buffer = buffer.mid(newlineIndex + 1);

        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (!doc.isObject()) continue;

        QJsonObject obj = doc.object();
        if (obj["type"].toString() == "lobby_update") {
            QStringList players;
            QJsonObject playerList = obj["players"].toObject();
            for (const QString &key : playerList.keys()) {
                players << QString("%1:%2").arg(key, playerList[key].toString());
            }

            emit lobbyUpdated(players);
            qDebug() << "Lobby updated:" << players;
        }
    }
}

void GameClient::onError(QAbstractSocket::SocketError socketError)
{
    qWarning() << "Socket error:" << socketError << socket->errorString();
}





