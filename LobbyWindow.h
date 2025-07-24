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
    void exitRequested();
private slots:
    void handleExitButton();
public:
    Ui::LobbyWindow *ui;
private:

};

#endif // LOBBYWINDOW_H
