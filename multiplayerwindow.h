#ifndef MULTIPLAYERWINDOW_H
#define MULTIPLAYERWINDOW_H

#include "ui_MultiplayerWindow.h"
#include <QMainWindow>

class MultiplayerWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MultiplayerWindow(QWidget *parent = nullptr);
    ~MultiplayerWindow();

    void setAsHost();
    void setAsClient();

private:
    Ui::MultiplayerWindow *ui;
};

#endif // MULTIPLAYERWINDOW_H
