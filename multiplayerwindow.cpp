#include "MultiplayerWindow.h"
#include "ui_MultiplayerWindow.h"

#include "AnswerValidationDialog.h"

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
