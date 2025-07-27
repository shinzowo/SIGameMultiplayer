#include "GameLogic.h"
#include <QDebug>
GameLogic::GameLogic(QObject* parent)
    : QObject(parent) {}

void GameLogic::startGame(const QList<Player>& players, const QString& questionFilePath) {
    m_players = players;
    m_package.loadFromFile(questionFilePath);
    m_currentRoundIndex = 0;
}

int GameLogic::getCurrentRoundIndex() const {
    return m_currentRoundIndex;
}

void GameLogic::nextRound() {
    if (m_currentRoundIndex + 1 < m_package.getRoundCount()) {
        m_currentRoundIndex++;
    }
}

const QList<Player>& GameLogic::getPlayers() const {
    return m_players;
}

Player& GameLogic::getPlayer(int index) {
    return m_players[index];
}

int GameLogic::getPlayerCount() const {
    return m_players.size();
}

GameRound GameLogic::getCurrentRound() const {
    if (m_currentRoundIndex >= m_package.getRoundCount()) {
        qWarning() << "getCurrentRound(): index out of range:" << m_currentRoundIndex;
        return GameRound(); // вернёт пустой объект
    }
    return m_package.getRound(m_currentRoundIndex);
}


Question GameLogic::getQuestion(int themeIndex, int questionIndex) const {
    GameRound round = getCurrentRound();
    if (themeIndex >= 0 && themeIndex < round.themes.size()) {
        const Theme& theme = round.themes[themeIndex];
        if (questionIndex >= 0 && questionIndex < theme.questions.size()) {
            return theme.questions[questionIndex];
        }
    }
    return Question(); // пустой вопрос
}

void GameLogic::applyAnswer(int playerIndex, bool correct, int cost) {
    if (playerIndex >= 0 && playerIndex < m_players.size()) {
        if (correct) {
            m_players[playerIndex].addScore(cost);
        } else {
            m_players[playerIndex].addScore(-cost);
        }
    }
}

bool GameLogic::isFinalRound() const {
    return getCurrentRound().name.toLower().contains("финал");
}

bool GameLogic::isGameOver() const {
    return m_currentRoundIndex >= m_package.getRoundCount() - 1;
}

void GameLogic::markQuestionAnswered(int themeIndex, int questionIndex) {
    if (themeIndex >= 0 && themeIndex < m_package.getRound(m_currentRoundIndex).themes.size()) {
        Theme& theme = m_package.getRound(m_currentRoundIndex).themes[themeIndex];
        if (questionIndex >= 0 && questionIndex < theme.questions.size()) {
            theme.questions[questionIndex].answered = true;
        }
    }
}


bool GameLogic::allQuestionsAnswered() const {
    const GameRound& round = m_package.getRound(m_currentRoundIndex);
    for (const auto& theme : round.themes) {
        for (const auto& q : theme.questions) {
            if (!q.answered) {
                return false;
            }
        }
    }
    return true;
}

void GameLogic::updateScore(const QString &playerName, int points) {
    for (Player &p : m_players) {
        if (p.getName() == playerName) {
            p.addScore(points);
            qDebug() << "Score updated:" << p.getName() << "->" << points;
            break;
        }
    }
}




