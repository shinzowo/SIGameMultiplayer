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

// В классе GameServer добавить приватные поля
QString playerWhoBuzzed;  // Ник игрока, который первый нажал "buzz"
bool buzzActive = false;  // Флаг, что сейчас ожидается ответ после базза

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
            QString nickname = obj["nickname"].toString();
            bool isReady = obj["is_ready"].toBool();
            readyPlayers[nickname] = isReady;

            checkAllReady();
            broadcastLobby();
        }
        else if (type == "question_selected") {
            int themeIndex = obj["themeIndex"].toInt();
            int questionIndex = obj["questionIndex"].toInt();
            QString byPlayer = obj["nickname"].toString();

            if (byPlayer != hostNickname) {
                qDebug() << "Non-host tried to select question";
                return;
            }

            Question question = gameLogic.getQuestion(themeIndex, questionIndex);

            if (question.answered) {
                qDebug() << "Question already answered:" << themeIndex << questionIndex;
                continue;
            }

            // Отмечаем вопрос отвеченным
            gameLogic.markQuestionAnswered(themeIndex, questionIndex);

            // Формируем JSON для отправки вопроса всем клиентам
            QJsonObject questionJson;
            questionJson["cost"] = question.cost;
            questionJson["text"] = question.text;
            questionJson["answer"] = question.answer;
            questionJson["type"] = question.type;
            questionJson["answered"] = question.answered;

            QJsonObject broadcast;
            broadcast["type"] = "question_started";
            broadcast["themeIndex"] = themeIndex;
            broadcast["questionIndex"] = questionIndex;
            broadcast["by"] = byPlayer;
            broadcast["question"] = questionJson;

            QByteArray bData = QJsonDocument(broadcast).toJson(QJsonDocument::Compact) + "\n";
            for (QTcpSocket *socket : clients.keys()) {
                if (socket->state() == QAbstractSocket::ConnectedState)
                    socket->write(bData);
            }

            buzzActive = true;     // Разрешаем кнопки buzzer для игроков
            playerWhoBuzzed.clear();
        }
        else if (type == "buzz") {
            if (!buzzActive) {
                qDebug() << "Buzz ignored, not active phase";
                continue;
            }
            QString nickname = clients[client];
            if (playerWhoBuzzed.isEmpty()) {
                playerWhoBuzzed = nickname;
                buzzActive = false;

                QJsonObject buzzMsg;
                buzzMsg["type"] = "player_buzzed";
                buzzMsg["nickname"] = nickname;
                QByteArray bData = QJsonDocument(buzzMsg).toJson(QJsonDocument::Compact) + "\n";
                for (QTcpSocket *sock : clients.keys()) {
                    if (sock->state() == QAbstractSocket::ConnectedState)
                        sock->write(bData);
                }

                qDebug() << "Player buzzed:" << nickname;
            } else {
                qDebug() << "Buzz ignored, already buzzed by" << playerWhoBuzzed;
            }
        }
        else if (type == "answer_validated") {
            if (playerWhoBuzzed.isEmpty()) {
                qDebug() << "No player buzzed, ignoring answer_validated";
                continue;
            }

            bool correct = obj["correct"].toBool();

            int themeIndex = obj["themeIndex"].toInt();
            int questionIndex = obj["questionIndex"].toInt();

            // Получаем стоимость вопроса (очки)
            Question question = gameLogic.getQuestion(themeIndex, questionIndex);
            int points = question.cost;

            if (correct) {
                gameLogic.updateScore(playerWhoBuzzed, points);
            } else {
                gameLogic.updateScore(playerWhoBuzzed, -points);  // штраф
            }


            buzzActive = false;

            sendGameData();

            qDebug() << "Answer validated for" << playerWhoBuzzed << "correct:" << correct;
            playerWhoBuzzed.clear();
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
    qDebug() << "sendGameData() called";

    QJsonObject msg;
    msg["type"] = "game_data";
    msg["title"] = gameLogic.getCurrentRound().name;

    qDebug() << "Current round title:" << msg["title"].toString();

    QJsonArray themesArray;
    QJsonArray questionsArray;

    const GameRound &round = gameLogic.getCurrentRound();
    int questionId = 1;

    for (int t = 0; t < round.themes.size(); ++t) {
        const Theme &theme = round.themes[t];
        themesArray.append(theme.name);

        qDebug() << "Theme[" << t << "]:" << theme.name << "with" << theme.questions.size() << "questions";

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

            qDebug() << "  Question[" << q << "] cost:" << ques.cost << "answered:" << ques.answered
                     << "text:" << ques.text.left(30);  // Показываем первые 30 символов
        }
    }

    QJsonArray playerArray;

    const auto &players = gameLogic.getPlayers();

    qDebug() << "Players count:" << players.size() << ", Host:" << hostNickname;

    // Добавляем ведущего первым
    bool hostAdded = false;
    for (const Player &p : players) {
        if (p.getName() == hostNickname) {
            QJsonObject pObj;
            pObj["name"] = p.getName();
            pObj["score"] = p.getScore();
            playerArray.append(pObj);
            hostAdded = true;
            qDebug() << "Added host player:" << p.getName() << "score:" << p.getScore();
            break;
        }
    }
    if (!hostAdded) {
        qWarning() << "Host player" << hostNickname << "not found in players list!";
    }

    // Добавляем остальных
    for (const Player &p : players) {
        if (p.getName() != hostNickname) {
            QJsonObject pObj;
            pObj["name"] = p.getName();
            pObj["score"] = p.getScore();
            playerArray.append(pObj);
            qDebug() << "Added player:" << p.getName() << "score:" << p.getScore();
        }
    }

    msg["themes"] = themesArray;
    msg["questions"] = questionsArray;
    msg["players"] = playerArray;

    QJsonDocument doc(msg);
    QByteArray data = doc.toJson(QJsonDocument::Compact) + "\n";

    qDebug() << "Broadcasting game_data to clients, data size:" << data.size();
    qDebug() << "JSON data:" << data;

    for (QTcpSocket *socket : clients.keys()) {
        if (socket->state() == QAbstractSocket::ConnectedState) {
            qDebug() << "Sending data to client:" << clients[socket];
            socket->write(data);
        } else {
            qDebug() << "Socket not connected:" << clients[socket];
        }
    }
}

void GameServer::setQuestionFilePath(QString filePath){
    questionFilePath=filePath;
}







