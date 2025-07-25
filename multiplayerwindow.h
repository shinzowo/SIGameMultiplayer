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
    void updatePlayersUI(const QJsonArray& players);
    void showQuestionDialog(const QString &question, int time);
    void showAnswerValidationDialog(const QString& question, const QString& answer);
public slots:
    void onGameDataReceived(const QString& title,
                            const QJsonArray& players,
                            const QJsonArray& themes,
                            const QJsonArray& questions);



private:
    Ui::MultiplayerWindow *ui;
};

#endif // MULTIPLAYERWINDOW_H
