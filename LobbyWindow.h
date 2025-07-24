#ifndef LOBBYWINDOW_H
#define LOBBYWINDOW_H

#include "ui_LobbyWindow.h"
#include <QMainWindow>

class LobbyWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit LobbyWindow(QWidget *parent = nullptr);
    ~LobbyWindow();

signals:
    void readyStatusChanged(bool isReady);
    void exitRequested();
    void startGameRequested();
private slots:
    void handleExitButton();
    void handleReadyButton();
public:
    Ui::LobbyWindow *ui;
private:
    bool isReady = false;
};

#endif // LOBBYWINDOW_H
