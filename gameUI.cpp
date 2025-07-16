#include "gameUI.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QInputDialog>

GameWidget::GameWidget(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    font=new QFont("Inter", 16);
    QString style = R"(
           QPushButton {
               background-color: #c9d052;
               color: black;
               border: 1px solid #000000;
               border-radius: 10px;
           }
           QPushButton:hover {
               background-color: #A8AE43;
           }
           QPushButton:pressed {
               background-color: #1f618d;
           }
    )";

    QHBoxLayout *menuLayout = new QHBoxLayout(this);
    mainLayout->addLayout(menuLayout);
    backButton = new QPushButton("Назад");
    backButton->setFixedSize(QSize(100, 35));
    menuLayout->addWidget(backButton);
    menuLayout->addStretch();
    backButton->setStyleSheet(style);
    backButton->setFont(*font);
    connect(backButton, &QPushButton::clicked, this, &GameWidget::onBackClicked);

    //секция игроков
    playersLayout=new QHBoxLayout();
    mainLayout->addLayout(playersLayout);

    //сетка вопросов
    m_gridLayout = new QGridLayout();
    mainLayout->addLayout(m_gridLayout);

}

void GameWidget::setLogic(GameLogic* logic) {
    m_logic = logic;
    players = m_logic->getPlayers();

    playerNameLabels.clear();
    playerScoreLabels.clear();
    // playerButtons.clear(); // больше не используется

    if (playersLayout) {
        QLayoutItem* child;
        while ((child = playersLayout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
    } else {
        playersLayout = new QHBoxLayout();
        static_cast<QVBoxLayout*>(layout())->addLayout(playersLayout);
    }

    for (int i = 0; i < players.size(); ++i) {
        const Player& p = players[i];

        QVBoxLayout* pl = new QVBoxLayout();

        QLabel* nameLabel = new QLabel(p.getName());
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setStyleSheet("padding: 6px; border: 1px solid gray;");
        nameLabel->setFont(*font);

        // Можно сохранить указатель, чтобы позже подсвечивать активного игрока
        playerNameLabels.append(nameLabel);

        QLabel* scoreLabel = new QLabel(QString::number(p.getScore()));
        scoreLabel->setAlignment(Qt::AlignCenter);
        scoreLabel->setFont(*font);
        playerScoreLabels.append(scoreLabel);

        pl->addWidget(nameLabel);
        pl->addWidget(scoreLabel);

        playersLayout->addLayout(pl);
    }

    displayRound(m_logic->getCurrentRoundIndex());
}


void GameWidget::updatePlayerScores() {
    players = m_logic->getPlayers();
    for (int i = 0; i < players.size() && i < playerScoreLabels.size(); ++i) {
        playerScoreLabels[i]->setText(QString::number(players[i].getScore()));
    }
}



void GameWidget::displayRound(int roundIndex) {
    QString style = R"(
        QPushButton {
            background-color: #c9d052;
            color: #000000;
            font-weight: bold;
            font-size: 16px;
            padding: 10px 20px;
            border: 2px solid #8e9b2e;
            border-radius: 12px;
            transition: all 0.2s ease-in-out;
        }
        QPushButton:hover {
            background-color: #b0b842;
            border-color: #727b22;
        }
        QPushButton:pressed {
            background-color: #1f618d;
            color: white;
            border-color: #154360;
        }
        QPushButton:disabled {
            background-color: #e0e0e0;
            color: #888888;
            border-color: #bbbbbb;
        }
    )";

    if (!m_logic) return;

    clearGrid();
    GameRound round = m_logic->getCurrentRound();

    int themeCount = round.themes.size();
    int questionCount = themeCount > 0 ? round.themes[0].questions.size() : 0;

    questionButtons.clear();
    questionButtons.resize(themeCount);

    // Темы по вертикали в первом столбце
    for (int row = 0; row < themeCount; ++row) {
        QLabel* themeLabel = new QLabel(round.themes[row].name);
        themeLabel->setAlignment(Qt::AlignCenter);
        themeLabel->setFont(*font);
        m_gridLayout->addWidget(themeLabel, row, 0);  // вертикально в 0-й столбец
    }

    // Вопросы — по горизонтали от каждой темы
    for (int row = 0; row < themeCount; ++row) {
        const auto& questions = round.themes[row].questions;
        for (int col = 0; col < questions.size(); ++col) {
            const Question& q = questions[col];

            QPushButton* btn = new QPushButton(QString::number(q.cost));
            btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            btn->setProperty("theme", row);
            btn->setProperty("index", col);
            btn->setStyleSheet(style);
            connect(btn, &QPushButton::clicked, this, &GameWidget::handleQuestionClicked);

            m_gridLayout->addWidget(btn, row, col + 1);  // вправо от темы
            questionButtons[row].append(btn);
        }
    }
    updatePlayerScores();
}


void GameWidget::clearGrid() {
    if (!m_gridLayout) return;

    QLayoutItem* item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    questionButtons.clear();
}

void GameWidget::handleQuestionClicked() {
    if (!m_logic) return;

    QPushButton* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    int themeIndex = btn->property("theme").toInt();
    int questionIndex = btn->property("index").toInt();

    currentQuestion = m_logic->getQuestion(themeIndex, questionIndex);
    currentQuestionButton = btn;

    // Показываем текст вопроса
    QMessageBox::information(this, "Вопрос", QString("На %1 очков:\n\n%2")
                             .arg(currentQuestion.cost).arg(currentQuestion.text));

    // Сброс попыток
    hasAttempted = QVector<bool>(players.size(), false);

    // Переходим к выбору игрока
    askWhoWillAnswer();
    m_logic->markQuestionAnswered(themeIndex,questionIndex);
    if(m_logic->allQuestionsAnswered()){
        m_logic->nextRound();
        displayRound(m_logic->getCurrentRoundIndex());
    }
}

void GameWidget::askWhoWillAnswer() {
    QDialog dialog(this);
    dialog.setWindowTitle("Кто будет отвечать?");
    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QLabel* label = new QLabel(currentQuestion.text+"\n\n"+"Выберите игрока, кто будет отвечать:");
    layout->addWidget(label);

    QVector<QPushButton*> choiceButtons;

    for (int i = 0; i < players.size(); ++i) {
        if (!hasAttempted[i]) {
            QPushButton* btn = new QPushButton(players[i].getName());
            layout->addWidget(btn);
            connect(btn, &QPushButton::clicked, [&dialog, this, i]() {
                currentPlayerIndex = i;
                dialog.accept();
            });
            choiceButtons.append(btn);
        }
    }

    QPushButton* passBtn = new QPushButton("Никто не хочет отвечать");
    layout->addWidget(passBtn);
    connect(passBtn, &QPushButton::clicked, [&dialog]() {
        dialog.reject();
    });

    int result = dialog.exec();

    if (result == QDialog::Accepted) {

        askPlayerToAnswer();
    } else {
        // Никто не хочет отвечать
        currentQuestionButton->setEnabled(false);
        currentQuestionButton->setText("—");

    }
}


void GameWidget::askPlayerToAnswer() {
    const QString& name = players[currentPlayerIndex].getName();
    QString answer = QInputDialog::getText(this, "Ответ", currentQuestion.text+"\n\n" + name + ", введите ваш ответ:").trimmed();
    QString correctAnswer = currentQuestion.answer.trimmed();

    bool correct = answer.compare(correctAnswer, Qt::CaseInsensitive) == 0;

    if (correct) {
        QMessageBox::information(this, "Верно", "Ответ правильный! Баллы начислены.");
        m_logic->applyAnswer(currentPlayerIndex, true, currentQuestion.cost);
        updatePlayerScores();
        currentQuestionButton->setEnabled(false);
        currentQuestionButton->setText("—");

    } else {
        QMessageBox::warning(this, "Неверно", "Ответ неправильный. Баллы сняты.");
        m_logic->applyAnswer(currentPlayerIndex, false, currentQuestion.cost);
        updatePlayerScores();
        hasAttempted[currentPlayerIndex] = true;

        askWhoWillAnswer();  // Даем другим игрокам шанс
    }
}
