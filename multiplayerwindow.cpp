#include "MultiplayerWindow.h"
#include "ui_MultiplayerWindow.h"
#include <QMessageBox>
#include <QThread>



MultiplayerWindow::MultiplayerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MultiplayerWindow)
{
    ui->setupUi(this);
    connectQuestionButtons();
}

MultiplayerWindow::~MultiplayerWindow() {
    delete ui;
}

void MultiplayerWindow::setClient(GameClient* c) {
    client = c;
    connect(client, &GameClient::jsonReceived, this, &MultiplayerWindow::onServerMessageReceived);

}
void MultiplayerWindow::setNickname(const QString& name) { nickname = name; }

void MultiplayerWindow::setAsHost() {
    isHost=true;
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
    isHost=false;
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
    qDebug()<<"onGameDataReceived is called";
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
void MultiplayerWindow::updatePlayersUI(const QJsonArray& players) {
    qDebug() << "--- UI UPDATE START ---";
    qDebug() << "Current thread:" << QThread::currentThread();

    // Проверка входных данных
    qDebug() << "Players data:";
    for (const QJsonValue &p : players) {
        auto obj = p.toObject();
        qDebug() << "  " << obj["name"].toString() << obj["score"].toInt();
    }

    // Обновление ведущего
    if (!players.isEmpty()) {
        QJsonObject host = players[0].toObject();
        qDebug() << "Updating host label:" << host["name"].toString();
        ui->labelNickname->setText("Ведущий: " + host["name"].toString());
    }

    // Обновление игроков
    for (int i = 1; i < 5; ++i) {
        QString boxName = QString("player%1Box").arg(i);
        QString scoreName = QString("player%1Score").arg(i);

        QGroupBox* box = findChild<QGroupBox*>(boxName);
        QLabel* score = findChild<QLabel*>(scoreName);

        qDebug() << "\nPlayer" << i << "elements:";
        qDebug() << "Box:" << boxName << "found:" << (box != nullptr);
        qDebug() << "Score:" << scoreName << "found:" << (score != nullptr);

        if (i < players.size()) {
            QJsonObject p = players[i].toObject();
            QString name = p["name"].toString();
            int scr = p["score"].toInt();

            qDebug() << "Setting values - Name:" << name << "Score:" << scr;

            if (box) {
                box->setTitle(name);
                box->show();
                qDebug() << "Box title set to:" << box->title();
            }

            if (score) {
                score->setText(QString::number(scr));
                score->show();
                qDebug() << "Score text set to:" << score->text();
            }
        } else {
            qDebug() << "Hiding player" << i << "elements";
            if (box) box->hide();
            if (score) score->hide();
        }
    }

    qDebug() << "--- UI UPDATE COMPLETE ---";
    QApplication::processEvents(); // Принудительная обработка событий
}



void MultiplayerWindow::showQuestionDialog(const QString &question, int time) {
    if (questionDialog) {
        questionDialog->close();
        delete questionDialog;
    }

    questionDialog = new QuestionDialog(this);
    questionDialog->setWindowFlags(questionDialog->windowFlags() | Qt::WindowStaysOnTopHint);
    questionDialog->setQuestionText(question);
    questionDialog->startCountdown(time);

    answerAccepted = false;
    ui->answerButton->setEnabled(true);

    connect(questionDialog, &QuestionDialog::timeout, this, [=]() {
        if (client) {
            QJsonObject msg;
            msg["type"] = "question_timeout";
            client->sendJson(msg);
        }
        questionDialog->close();
    });

    connect(ui->answerButton, &QPushButton::clicked, this, [=]() {
        if (!answerAccepted) {
            answerAccepted = true;
            ui->answerButton->setEnabled(false);
            if (client) {
                QJsonObject msg;
                msg["type"] = "player_answer_attempt";
                msg["nickname"] = nickname;  // Предполагается, что nickname есть
                client->sendJson(msg);
            }
        }
    });

    questionDialog->show();
}



void MultiplayerWindow::onQuestionSelected(int themeIndex, int questionIndex) {
    QJsonObject msg;
    msg["type"] = "question_selected";
    msg["themeIndex"] = themeIndex;
    msg["questionIndex"] = questionIndex;
    client->sendJson(msg);
}

void MultiplayerWindow::handleQuestionStart(const QJsonObject &msg) {
    currentQuestion = msg; // сохраним для оценки
    if(!isHost){
        showQuestionDialog(msg["text"].toString(), 20);
    }
    ui->answerButton->setEnabled(false);
}

void MultiplayerWindow::connectQuestionButtons()
{

    QLayout* layout = ui->gridLayout;

    if (!layout) {
        qWarning() << "Question grid layout not found!";
        return;
    }

    for (int i = 0; i < layout->count(); ++i) {
        QLayoutItem* item = layout->itemAt(i);
        if (!item) continue;

        QWidget* widget = item->widget();
        if (!widget) continue;

        QPushButton* button = qobject_cast<QPushButton*>(widget);
        if (!button) continue;

        // Подключаем слот на нажатие
        connect(button, &QPushButton::clicked, this, &MultiplayerWindow::onQuestionButtonClicked);
    }
}

void MultiplayerWindow::onQuestionButtonClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;


    int themeIndex = button->property("themeIndex").toInt();
    int questionIndex = button->property("questionIndex").toInt();

    currentThemeIndex = themeIndex;
    currentQuestionIndex = questionIndex;

    if (client) {
        QJsonObject msg;
        msg["type"] = "question_selected";
        msg["themeIndex"] = themeIndex;
        msg["questionIndex"] = questionIndex;
        msg["nickname"] = nickname;  // твоя переменная с ником
        client->sendJson(msg);
    }
}


void MultiplayerWindow::handleEnableBuzz() {
    ui->answerButton->setEnabled(true);
}

void MultiplayerWindow::on_answerButton_clicked() {
    QJsonObject msg;
    msg["type"] = "buzz";
    msg["nickname"] = nickname;
    client->sendJson(msg);
    ui->answerButton->setEnabled(false);
}

void MultiplayerWindow::onServerMessageReceived(const QJsonObject& obj)
{
    qDebug()<<"onServerMessageReceived is called";
    QString type = obj["type"].toString();

    if (type == "question_started") {
        QJsonObject question = obj["question"].toObject();
        QString text = question["text"].toString();
        int cost = question["cost"].toInt();

        currentQuestionText=text;
        currentCorrectAnswer=question["answer"].toString();
        if(!isHost){
            showQuestionDialog(text, 7);
        }

    }
    else if (type == "player_buzzed") {
        QString player = obj["nickname"].toString();
        playerWhoBuzzed=player;
        // Только ведущий показывает диалог подтверждения
        if (isHost) {

            qDebug()<<"Show questionText and etc:"<<currentQuestionText<<" "<<currentCorrectAnswer;
            showAnswerValidationDialog(currentQuestionText, currentCorrectAnswer);
        }

        // Можно показать на экране, кто нажал
        //QMessageBox::information(this, "Ответ", QString("%1 нажал кнопку!").arg(player));
        return;
    }
    else if (type == "answer_validated") {
        bool correct = obj["correct"].toBool();

        // Можно отключить кнопку ответа
        ui->answerButton->setEnabled(false);

        QString msg = correct ? "Ответ верный!" : "Ответ неверный!";
        QMessageBox::information(this, "Результат", msg);
        return;
    }
    else if (type == "game_data") {
        QString title = obj["title"].toString();
        QJsonArray themes = obj["themes"].toArray();
        QJsonArray questions = obj["questions"].toArray();
        QJsonArray players = obj["players"].toArray();

        onGameDataReceived(title, players, themes, questions);
    }
    else if (type == "question_timeout") {
        QMessageBox::information(this, "Время вышло", "Никто не ответил вовремя!");
        return;
    }

}


void MultiplayerWindow::handlePlayerBuzzed(const QString &nickname) {
    ui->answerButton->setEnabled(false);

}

void MultiplayerWindow::handleAnswerSubmitted(const QJsonObject &msg) {
    if (isHost) {
        showAnswerValidationDialog(msg["question"].toString(), msg["answer"].toString());
    }
}

void MultiplayerWindow::showAnswerValidationDialog(const QString &question, const QString &answer) {
    auto dialog = new AnswerValidationDialog(this);
    dialog->setQuestionAndAnswer(question, answer);

    connect(dialog, &AnswerValidationDialog::answerEvaluated, this, [=](bool correct) {
        QJsonObject msg;
        msg["type"] = "answer_validated";
        msg["correct"] = correct;
        msg["themeIndex"] = currentThemeIndex;
        msg["questionIndex"] = currentQuestionIndex;
        msg["player"] = playerWhoBuzzed;
        client->sendJson(msg);

    });
    qDebug()<<currentThemeIndex<<" "<<currentQuestionIndex;
    dialog->exec();
}

void MultiplayerWindow::setButtonsEnabledForHost(bool enabled) {
    QLayout* layout = ui->gridLayout;
    for (int i = 0; i < layout->count(); ++i) {
        QLayoutItem* item = layout->itemAt(i);
        if (!item) continue;
        QPushButton* btn = qobject_cast<QPushButton*>(item->widget());
        if (btn) btn->setEnabled(enabled);
    }
}





