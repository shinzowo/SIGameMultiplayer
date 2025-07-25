#include "MultiplayerWindow.h"
#include "ui_MultiplayerWindow.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "answervalidationdialog.h"
#include "questiondialog.h"

MultiplayerWindow::MultiplayerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MultiplayerWindow)
{
    ui->setupUi(this);
}

MultiplayerWindow::~MultiplayerWindow() {
    delete ui;
}


void MultiplayerWindow::setAsHost() {
    ui->answerButton->setVisible(false);
    ui->answerButton->setEnabled(false);
    ui->addScore1->setVisible(true);
    ui->addScore2->setVisible(true);
    ui->addScore3->setVisible(true);
    ui->addScore4->setVisible(true);
    ui->deductScore1->setVisible(true);
    ui->deductScore2->setVisible(true);
    ui->deductScore3->setVisible(true);
    ui->deductScore4->setVisible(true);

}

void MultiplayerWindow::setAsClient() {
    ui->answerButton->setVisible(true);
    ui->addScore1->setVisible(false);
    ui->addScore2->setVisible(false);
    ui->addScore3->setVisible(false);
    ui->addScore4->setVisible(false);
    ui->deductScore1->setVisible(false);
    ui->deductScore2->setVisible(false);
    ui->deductScore3->setVisible(false);
    ui->deductScore4->setVisible(false);
}

void MultiplayerWindow::onGameDataReceived(const QString& title,
                                           const QJsonArray& players,
                                           const QJsonArray& themes,
                                           const QJsonArray& questions)
{
    updatePlayersUI(players);


    for (int row = 0; row < themes.size(); ++row) {
        QWidget *item = ui->gridLayout->itemAtPosition(row, 0)->widget();
        if (QLabel *label = qobject_cast<QLabel*>(item)) {
            label->setText(themes[row].toString());
            label->show();
        }
    }




    for (const QJsonValue &v : questions) {
        QJsonObject q = v.toObject();
        int themeIndex = q["themeIndex"].toInt();      // row
        int cost = q["cost"].toInt();                  // 100, 200, ...
        int col = cost / 100;                          // 1 = 100, 2 = 200, ..., 5 = 500
        QString text = q["text"].toString();

        QWidget *item = ui->gridLayout->itemAtPosition(themeIndex, col)->widget();
        if (QPushButton *btn = qobject_cast<QPushButton*>(item)) {
            btn->setText(QString::number(cost));
            btn->setEnabled(!q["answered"].toBool());
            btn->setProperty("questionText", text);
            btn->setProperty("themeIndex", themeIndex);
            btn->setProperty("questionIndex", q["questionIndex"].toInt()); // если нужно
            btn->show();
        }
    }



}
void MultiplayerWindow::updatePlayersUI(const QJsonArray& players)
{
    const int maxPlayers = 5;
    int visiblePlayers = players.size();

    // Первый игрок — ведущий, его отобразим отдельно
    if (visiblePlayers > 0) {
        QJsonObject host = players[0].toObject();
        ui->labelNickname->setText("Ведущий: " + host["name"].toString());
    }

    // Игроки (начиная со второго)
    for (int i = 1; i < maxPlayers; ++i) {
        QGroupBox* box = findChild<QGroupBox*>(QString("player%1Box").arg(i));
        QLabel* score = findChild<QLabel*>(QString("player%1Score").arg(i));

        if (i < visiblePlayers) {
            QJsonObject p = players[i].toObject();
            QString name = p["name"].toString();
            int scr = p["score"].toInt();

            if (box) box->setTitle(name);
            if (score) score->setText(QString::number(scr));

            if (box) box->show();
        } else {
            if (box) box->hide();  // Скрываем лишние
        }
    }
}

void MultiplayerWindow::showAnswerValidationDialog(const QString& question, const QString& answer) {
    auto dialog = new AnswerValidationDialog(this);
    dialog->setQuestionAndAnswer(question, answer);

    connect(dialog, &AnswerValidationDialog::answerEvaluated, this, [=](bool correct) {
        // Обработка ответа ведущим (true/false)
        qDebug() << "Ведущий оценил ответ как:" << (correct ? "Правильный" : "Неправильный");
        // Здесь можешь отправить результат на сервер
    });

    dialog->exec(); // показать модально
}

void MultiplayerWindow::showQuestionDialog(const QString &question, int time) {
    QuestionDialog* dialog = new QuestionDialog(this);
    dialog->setWindowFlags(dialog->windowFlags() | Qt::WindowStaysOnTopHint);
    dialog->show();
    dialog->setQuestionText(question);
    dialog->startCountdown(time);

    connect(dialog, &QuestionDialog::timeout, this, [=]() {
        // Например, отправка серверу, что время вышло
        // sendAnswerTimeoutToServer();
    });

    dialog->exec();
}



