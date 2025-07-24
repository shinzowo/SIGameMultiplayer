#include "QuestionDialog.h"
#include "ui_QuestionDialog.h"

QuestionDialog::QuestionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuestionDialog),
    timer(new QTimer(this))
{
    ui->setupUi(this);
    connect(timer, &QTimer::timeout, this, &QuestionDialog::updateTimer);
}

QuestionDialog::~QuestionDialog() {
    delete ui;
}

void QuestionDialog::setQuestionText(const QString &question) {
    ui->questionLabel->setText(question);
}

void QuestionDialog::startCountdown(int seconds) {
    timeLeft = seconds;
    ui->timerLabel->setText(QString::number(timeLeft));
    timer->start(1000);
}

void QuestionDialog::updateTimer() {
    timeLeft--;
    ui->timerLabel->setText(QString::number(timeLeft));
    if (timeLeft <= 0) {
        timer->stop();
        emit timeout();
        close(); // можно не закрывать, если сервер скажет ждать
    }
}
