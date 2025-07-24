#ifndef MENU_H
#define MENU_H
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
class MainMenuWidget : public QWidget {
    Q_OBJECT

public:
    explicit MainMenuWidget(QWidget *parent = nullptr);

signals:
    void startMultiplayerGameRequested();
    void startSingleGameRequested();
    void editQuestionRequested();
    void exitRequested();
public:
    QString getNickname();
    void saveNickname();
private:
    QString nickname;
    QLineEdit* nameEdit;
};
#endif // MENU_H

