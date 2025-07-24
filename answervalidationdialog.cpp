#include "AnswerValidationDialog.h"
#include "ui_AnswerValidationDialog.h"

AnswerValidationDialog::AnswerValidationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnswerValidationDialog)
{
    ui->setupUi(this);
}

AnswerValidationDialog::~AnswerValidationDialog() {
    delete ui;
}

void AnswerValidationDialog::setQuestionAndAnswer(const QString &question, const QString &answer) {
    ui->questionLabel->setText(question);
    ui->answerLabel->setText(answer);
}

void AnswerValidationDialog::on_yesButton_clicked() {
    emit answerEvaluated(true);
    close();
}

void AnswerValidationDialog::on_noButton_clicked() {
    emit answerEvaluated(false);
    close();
}
