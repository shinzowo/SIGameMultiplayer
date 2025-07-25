#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QObject>
#include "QuestionPackage.h"
#include "Player.h"

class GameLogic : public QObject {
    Q_OBJECT

public:
    explicit GameLogic(QObject* parent = nullptr);

    void startGame(const QList<Player>& players, const QString& questionFilePath);

    int getCurrentRoundIndex() const;
    void nextRound();

    const QList<Player>& getPlayers() const;
    Player& getPlayer(int index);

    int getPlayerCount() const;

    GameRound getCurrentRound() const;
    Question getQuestion(int themeIndex, int questionIndex) const;

    void applyAnswer(int playerIndex, bool correct, int cost);
    bool isFinalRound() const;
    bool isGameOver() const;
    void markQuestionAnswered(int themeIndex, int questionIndex);
    bool allQuestionsAnswered() const;


private:
    QList<Player> m_players;
    QuestionPackage m_package;
    int m_currentRoundIndex = 0;
    QString HostName;
};

#endif // GAMELOGIC_H
