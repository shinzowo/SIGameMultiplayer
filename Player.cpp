// player.cpp
#include "Player.h"

Player::Player(const QString &name) : name(name) {}

QString Player::getName() const {
    return name;
}

int Player::getScore() const {
    return score;
}

void Player::addScore(int points) {
    score += points;
}

void Player::subtractScore(int points) {
    score -= points;
}
