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

signals:
    void lobbyUpdated(const QStringList &players);

private slots:
    void onConnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError);

private:
    QTcpSocket *socket;
    QString nickname;
    QByteArray buffer;
};



