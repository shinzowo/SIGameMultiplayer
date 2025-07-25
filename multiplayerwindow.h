#ifndef MULTIPLAYERWINDOW_H
#define MULTIPLAYERWINDOW_H

#include "ui_MultiplayerWindow.h"
#include <QMainWindow>

#include "answervalidationdialog.h"
#include "questiondialog.h"
#include "gameClient.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

class MultiplayerWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MultiplayerWindow(QWidget *parent = nullptr);
    ~MultiplayerWindow();

    void setClient(GameClient* c);
    void setNickname(const QString& name);

    void setAsHost();
    void setAsClient();
    void updatePlayersUI(const QJsonArray& players);
    void showQuestionDialog(const QString &question, int time);
    void showAnswerValidationDialog(const QString& question, const QString& answer);
    void handleQuestionStart(const QJsonObject &msg);
    void handleEnableBuzz();
    void handlePlayerBuzzed(const QString &nickname);
    void handleAnswerSubmitted(const QJsonObject &msg);
    void connectQuestionButtons();
    void setButtonsEnabledForHost(bool enabled);


public slots:
    void onGameDataReceived(const QString& title,
                            const QJsonArray& players,
                            const QJsonArray& themes,
                            const QJsonArray& questions);
    void onQuestionSelected(int themeIndex, int questionIndex);
    void onQuestionButtonClicked();
    void on_answerButton_clicked();
    void onServerMessageReceived(const QJsonObject& obj);
signals:
    void answerEvaluatedByHost(bool correct);

private:
    Ui::MultiplayerWindow *ui;
    GameClient* client = nullptr;
    QString nickname;

    bool isHost=false;
    QuestionDialog* questionDialog = nullptr;
    bool answerAccepted = false;
    QJsonObject currentQuestion; // Хранит текущий вопрос, для валидации и отправки

};

#endif // MULTIPLAYERWINDOW_H
