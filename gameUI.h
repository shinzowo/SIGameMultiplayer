#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include "gamelogic.h"
#include "QLabel"

class GameWidget : public QWidget {
    Q_OBJECT

public:
    explicit GameWidget(QWidget* parent = nullptr);

    void setLogic(GameLogic* logic);
    void displayRound(int roundIndex);
signals:
    void onBackClicked();

private slots:
    void handleQuestionClicked();

private:
    GameLogic* m_logic = nullptr;
    QGridLayout* m_gridLayout = nullptr;
    QVector<QVector<QPushButton*>> questionButtons;

    void clearGrid();

    Question currentQuestion;
    QList<Player> players;
    QVector<bool> hasAttempted;
    QPushButton* currentQuestionButton = nullptr;
    QPushButton* backButton=nullptr;

    QHBoxLayout* playersLayout = nullptr;  // новый лэйаут для игроков
    int currentPlayerIndex = -1;
    QVector<QLabel*> playerNameLabels;
    QVector<QLabel*> playerScoreLabels;

    void updatePlayerScores();
    void askWhoWillAnswer();
    void askPlayerToAnswer();

    QFont* font;


};

#endif // GAMEWIDGET_H
