#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "menu.h"
#include "playerSetup.h"
#include "gameUI.h"
#include "GameLogic.h"
#include "ConnectionWidget.h"
#include "LobbyWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStackedWidget *stackedWidget;
    MainMenuWidget *menuWidget;
    PlayerSetupWidget *setupSinglePlayWidget;
    GameWidget *gameWidget;
    GameLogic *gameLogic;
    ConnectionWidget *connectionSetup;
    LobbyWindow *lobbyWindow;


private slots:
    void showConnectionSetup();
    void showPlayerSetup();
    void showEditQuestionPack();
    void startGame(const QStringList playerNames, const QString& filePath);
    void toMenu();
    void toPlayerSetup();
    void showLobby();

};
#endif // MAINWINDOW_H
