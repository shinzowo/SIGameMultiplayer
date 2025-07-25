#pragma once

#include <QTcpSocket>
#include <QObject>



class GameClient : public QObject
{
    Q_OBJECT
public:
    explicit GameClient(QObject *parent = nullptr);
    ~GameClient();
    void connectToServer(const QString &host, quint16 port, const QString &nickname);
    void sendReadyStatus(bool isReady);

signals:
    void lobbyUpdated(const QStringList &players);
    void gameStarted();
    void gameDataReceived(const QString& title,
                              const QJsonArray& players,
                              const QJsonArray& themes,
                              const QJsonArray& questions);

private slots:
    void onConnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError);

private:
    QTcpSocket *socket;
    QString nickname;
    QByteArray buffer;
    const QString READY_STATUS = "ready_status";
    const QString GAME_START = "game_start";
};



