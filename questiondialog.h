#pragma once
#include <QDialog>
#include <QTimer>

namespace Ui {
class QuestionDialog;
}

class QuestionDialog : public QDialog {
    Q_OBJECT

public:
    explicit QuestionDialog(QWidget *parent = nullptr);
    ~QuestionDialog();

    void setQuestionText(const QString &question);
    void startCountdown(int seconds);

signals:
    void timeout();

private slots:
    void updateTimer();

private:
    Ui::QuestionDialog *ui;
    QTimer *timer;
    int timeLeft;
};
