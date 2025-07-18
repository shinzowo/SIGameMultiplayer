#include "GameServer.h"
#include <QJsonDocument>
#include <QJsonObject>

GameServer::GameServer(QObject* parent) : QTcpServer(parent) {}

void GameServer::start(quint16 port) {
    if (!listen(QHostAddress::Any, port)) {
        qWarning() << "Server could not start!";
    } else {
        qDebug() << "Server started on port" << port;
    }
}

void GameServer::incomingConnection(qintptr socketDescriptor) {
    auto* socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &GameServer::onClientReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &GameServer::onClientDisconnected);

    m_clients[socket] = "Unknown";
    qDebug() << "New client connected:" << socketDescriptor;
}

void GameServer::onClientReadyRead() {
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    QByteArray data = client->readAll();
    handleMessage(client, QString::fromUtf8(data));
}

void GameServer::onClientDisconnected() {
    QTcpSocket* client = qobject_cast<QTcpSocket*>(sender());
    m_clients.remove(client);
    client->deleteLater();
}

void GameServer::handleMessage(QTcpSocket* client, const QString& message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) return;

    QJsonObject obj = doc.object();
    QString type = obj["type"].toString();

    if (type == "join") {
        QString name = obj["name"].toString();
        m_clients[client] = name;
        broadcastMessage(QString("Игрок %1 присоединился").arg(name));
    }

    // Можно обрабатывать другие типы: "answer", "selectQuestion", и т.д.
}

void GameServer::broadcastMessage(const QString& message) {
    for (QTcpSocket* client : m_clients.keys()) {
        client->write(message.toUtf8());
    }
}
