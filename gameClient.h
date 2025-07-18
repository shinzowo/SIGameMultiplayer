#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include <QTcpSocket>
#include <QObject>

class GameClient : public QObject {
    Q_OBJECT

public:
    explicit GameClient(QObject* parent = nullptr);
    void connectToServer(const QString& ip, quint16 port);
    void sendJoinRequest(const QString& playerName);

private slots:
    void onReadyRead();

private:
    QTcpSocket* m_socket;
};

#endif // GAMECLIENT_H
