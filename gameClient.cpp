#include "GameClient.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>

GameClient::GameClient(QObject* parent) : QObject(parent) {
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::readyRead, this, &GameClient::onReadyRead);
}

void GameClient::connectToServer(const QString& ip, quint16 port) {
    m_socket->connectToHost(ip, port);
}

void GameClient::sendJoinRequest(const QString& playerName) {
    QJsonObject obj;
    obj["type"] = "join";
    obj["name"] = playerName;

    QJsonDocument doc(obj);
    m_socket->write(doc.toJson());
}

void GameClient::onReadyRead() {
    QByteArray data = m_socket->readAll();
    qDebug() << "From server:" << data;
}
