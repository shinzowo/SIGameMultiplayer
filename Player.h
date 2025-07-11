// player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <QString>

class Player {
public:
    Player(const QString &name);

    QString getName() const;
    int getScore() const;
    void addScore(int points);
    void subtractScore(int points);

private:
    QString name;
    int score = 0;
    bool isActive = true;
};

#endif // PLAYER_H
