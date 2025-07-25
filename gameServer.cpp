#include "GameServer.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <QDebug>

GameServer::GameServer(QObject *parent)
    : QTcpServer(parent)
{}

bool GameServer::startServer(quint16 port)
{
    if (!listen(QHostAddress::Any, port)) {
        qWarning() << "Server failed to start:" << errorString();
        return false;
    }
    qDebug() << "Server started on port" << port;
    return true;
}

void GameServer::incomingConnection(qintptr socketDescriptor)
{
    auto *clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketDescriptor);

    connect(clientSocket, &QTcpSocket::readyRead, this, &GameServer::onClientReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &GameServer::onClientDisconnected);

    clients[clientSocket] = "Unknown";
    qDebug() << "Client connected from" << clientSocket->peerAddress().toString();
}

void GameServer::onClientDisconnected()
{
    auto *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    qDebug() << "Client disconnected:" << clients[client];
    clients.remove(client);
    client->deleteLater();
    broadcastLobby();
}

void GameServer::onClientReadyRead()
{
    auto *client = qobject_cast<QTcpSocket*>(sender());
    if (!client) return;

    static QMap<QTcpSocket*, QByteArray> buffers;
    buffers[client] += client->readAll();

    while (true) {
        int index = buffers[client].indexOf('\n');
        if (index == -1) break;

        QByteArray line = buffers[client].left(index);
        buffers[client].remove(0, index + 1);

        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (!doc.isObject()) {
            qWarning() << "Invalid JSON received:" << line;
            continue;
        }

        QJsonObject obj = doc.object();
        QString type = obj["type"].toString();
        if (type == "join") {
            QString nickname = obj["nickname"].toString();
            clients[client] = nickname;

            if (clients.size() == 1) {
                hostNickname = nickname;
            }

            qDebug() << "Client joined with nickname:" << nickname;
            broadcastLobby();
        }
        else if (type == "ready_status") {
            qDebug()<<"onClientReadyRead ready status";
            QString nickname = obj["nickname"].toString();
            bool isReady = obj["is_ready"].toBool();
            readyPlayers[nickname] = isReady;

            // Проверяем, все ли готовы
            checkAllReady();
            broadcastLobby();
        }
        else if (type == "question_selected") {
            int themeIndex = obj["themeIndex"].toInt();
            int questionIndex = obj["questionIndex"].toInt();
            QString byPlayer = obj["nickname"].toString();

            gameLogic.markQuestionAnswered(themeIndex, questionIndex);

            QJsonObject broadcast;
            broadcast["type"] = "question_locked";
            broadcast["themeIndex"] = themeIndex;
            broadcast["questionIndex"] = questionIndex;
            broadcast["by"] = byPlayer;

            QByteArray bData = QJsonDocument(broadcast).toJson(QJsonDocument::Compact) + "\n";
            for (QTcpSocket *socket : clients.keys()) {
                if (socket->state() == QAbstractSocket::ConnectedState)
                    socket->write(bData);
            }
        }

    }
}

void GameServer::broadcastLobby()
{
    QJsonObject message;
    message["type"] = "lobby_update";

    QJsonObject players;
    int idx = 1;
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        QString status = it.value();
        if (readyPlayers.contains(status) && readyPlayers[status]) {
            status += " (Ready)";
        }

        if (it.value() == hostNickname)
            players["Host"] = status;
        else
            players[QString("Player%1").arg(idx++)] = status;
    }

    message["players"] = players;
    QByteArray data = QJsonDocument(message).toJson(QJsonDocument::Compact) + "\n";

    for (QTcpSocket *socket : clients.keys()) {
        if (socket->state() == QAbstractSocket::ConnectedState)
            socket->write(data);
    }
}

void GameServer::checkAllReady()
{
    if (readyPlayers.size() < clients.size()) return;

    bool allReady = true;
    for (bool ready : readyPlayers.values()) {
        if (!ready) {
            allReady = false;
            break;
        }
    }

    if (allReady) {
        // Подготовка игроков
        QList<Player> players;
        for (const QString &nickname : clients.values()) {
            Player p(nickname);
            players.append(p);
        }

        // Инициализация логики
        gameLogic.startGame(players, questionFilePath);

        QJsonObject message;
        message["type"] = "game_start";
        QByteArray data = QJsonDocument(message).toJson(QJsonDocument::Compact) + "\n";

        for (QTcpSocket *socket : clients.keys()) {
            if (socket->state() == QAbstractSocket::ConnectedState)
                socket->write(data);
        }
        sendGameData();
    }
}

void GameServer::sendGameData()
{

    QJsonObject msg;
    msg["type"] = "game_data";
    msg["title"] = gameLogic.getCurrentRound().name;

    QJsonArray themesArray;
    QJsonArray questionsArray;

    const GameRound &round = gameLogic.getCurrentRound();
    int questionId = 1;

    for (int t = 0; t < round.themes.size(); ++t) {
        const Theme &theme = round.themes[t];
        themesArray.append(theme.name);

        for (int q = 0; q < theme.questions.size(); ++q) {
            const Question &ques = theme.questions[q];
            QJsonObject qObj;
            qObj["id"] = questionId++;
            qObj["themeIndex"] = t;
            qObj["questionIndex"] = q;
            qObj["cost"] = ques.cost;
            qObj["text"] = ques.text;
            qObj["type"] = ques.type;
            qObj["answered"] = ques.answered;
            questionsArray.append(qObj);
        }
    }

    QJsonArray playerArray;

    // Сначала добавим ведущего
    for (const Player &p : gameLogic.getPlayers()) {
        if (p.getName() == hostNickname) {
            QJsonObject pObj;
            pObj["name"] = p.getName();
            pObj["score"] = p.getScore();
            playerArray.append(pObj);
            break;
        }
    }

    // Затем всех остальных
    for (const Player &p : gameLogic.getPlayers()) {
        if (p.getName() != hostNickname) {
            QJsonObject pObj;
            pObj["name"] = p.getName();
            pObj["score"] = p.getScore();
            playerArray.append(pObj);
        }
    }


    msg["themes"] = themesArray;
    msg["questions"] = questionsArray;
    msg["players"] = playerArray;

    QByteArray data = QJsonDocument(msg).toJson(QJsonDocument::Compact) + "\n";

    for (QTcpSocket *socket : clients.keys()) {
        if (socket->state() == QAbstractSocket::ConnectedState)
            socket->write(data);
    }

}






