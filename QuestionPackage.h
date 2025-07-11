#ifndef QUESTIONPACKAGE_H
#define QUESTIONPACKAGE_H

#include <QString>
#include <QList>

// Вопрос
struct Question {
    int cost;
    QString text;
    QString answer;
    QString type = "standard"; // standard, auction, bag, final
    bool answered = false;
};

// Тема
struct Theme {
    QString name;
    QList<Question> questions;
};

// Раунд
struct GameRound {
    QString name;
    QList<Theme> themes;
};

// Пакет вопросов
class QuestionPackage {
public:
    QuestionPackage();

    bool loadFromFile(const QString &filePath);

    QList<GameRound> getRounds() const;
    GameRound& getRound(int index); // для изменения
    const GameRound& getRound(int index) const; // для чтения
    int getRoundCount() const;



private:
    QList<GameRound> rounds;
};

#endif // QUESTIONPACKAGE_H


