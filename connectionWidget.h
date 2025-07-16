#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class ConnectionWidget : public QWidget {
    Q_OBJECT

public:
    explicit ConnectionWidget(QWidget *parent = nullptr);

signals:
    void connectToServer(const QString& ip, quint16 port);
    void backToMenu();

private slots:
    void handleConnectClicked();

private:
    QLineEdit* ipEdit;
    QLineEdit* portEdit;
    QPushButton* connectButton;
    QPushButton* backButton;
};

#endif // CONNECTIONWIDGET_H
