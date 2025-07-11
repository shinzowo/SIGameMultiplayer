#ifndef MENU_H
#define MENU_H
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

class MainMenuWidget : public QWidget {
    Q_OBJECT

public:
    explicit MainMenuWidget(QWidget *parent = nullptr);

signals:
    void startSingleGameRequested();
    void editQuestionRequested();
    void exitRequested();
};
#endif // MENU_H

