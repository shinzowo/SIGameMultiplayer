#ifndef CONNECTIONWIDGET_H
#define CONNECTIONWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QStackedWidget>

class ConnectionWidget : public QWidget {
    Q_OBJECT
public:
    explicit ConnectionWidget(QWidget *parent = nullptr);

signals:
    void startServer(quint16 port);
    void connectToServer(const QString& ip, quint16 port);
    void backToMenu();

private slots:
    void handleActionClicked();
    void modeChanged(int index);

private:
    QComboBox* modeSelector;

    // Для сервера
    QWidget* serverPage;
    QLineEdit* serverPortEdit;
    QPushButton* serverActionButton;

    // Для клиента
    QWidget* clientPage;
    QLineEdit* clientIpEdit;
    QLineEdit* clientPortEdit;
    QPushButton* clientActionButton;

    QPushButton* backButton;

    QStackedWidget* stackedWidget;
};

#endif // CONNECTIONWIDGET_H


