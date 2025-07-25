#pragma once
#include <QDialog>

namespace Ui {
class AnswerValidationDialog;
}

class AnswerValidationDialog : public QDialog {
    Q_OBJECT

public:
    explicit AnswerValidationDialog(QWidget *parent = nullptr);
    ~AnswerValidationDialog();

    void setQuestionAndAnswer(const QString &question, const QString &answer);

signals:
    void answerEvaluated(bool correct);

private slots:
    void on_yesButton_clicked();
    void on_noButton_clicked();

private:
    Ui::AnswerValidationDialog *ui;
};
