#include "mainwindow.h"
#include <QPalette>
#include "./ui_mainwindow.h"
#include "gameeditorwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(1080, 768);
    setWindowTitle("Своя игра");

    QPalette pal = QPalette();

    //set background color
    pal.setColor(QPalette::Window, QColor("#3D72D4"));
    setAutoFillBackground(true);
    setPalette(pal);

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    //create pages
    menuWidget = new MainMenuWidget(this);
    setupSinglePlayWidget = new PlayerSetupWidget(this);
    gameWidget = new GameWidget(this);
    connectionSetup = new ConnectionWidget(this);
    lobbyWindow = new LobbyWindow(this);

    //add pages in widgets
    stackedWidget->addWidget(menuWidget);
    stackedWidget->addWidget(setupSinglePlayWidget);
    stackedWidget->addWidget(gameWidget);
    stackedWidget->addWidget(connectionSetup);
    stackedWidget->addWidget(lobbyWindow);

    connect(menuWidget, &MainMenuWidget::startMultiplayerGameRequested, this, &MainWindow::showConnectionSetup);
    connect(menuWidget, &MainMenuWidget::startSingleGameRequested, this, &MainWindow::showPlayerSetup);
    connect(menuWidget, &MainMenuWidget::editQuestionRequested, this, &MainWindow::showEditQuestionPack);
    connect(menuWidget, &MainMenuWidget::exitRequested, this, &MainWindow::close);

    connect(setupSinglePlayWidget, &PlayerSetupWidget::playersReady, this, &MainWindow::startGame);
    connect(setupSinglePlayWidget, &PlayerSetupWidget::onBackClicked, this, &MainWindow::toMenu);
    connect(connectionSetup, &ConnectionWidget::backToMenu, this, &MainWindow::toMenu);

    connect(gameWidget, &GameWidget::onBackClicked, this, &MainWindow::toPlayerSetup);
}
void MainWindow::showConnectionSetup(){
    stackedWidget->setCurrentWidget(connectionSetup);
}
void MainWindow::showEditQuestionPack(){
    GameEditorWindow *editor = new GameEditorWindow(this);
    editor->show();
}
void MainWindow::showPlayerSetup() {
    stackedWidget->setCurrentWidget(setupSinglePlayWidget);
}

void MainWindow::startGame(const QStringList playerNames, const QString& filePath) {
    // Можно передать список игроков в GameWidget, если нужно
    // gameWidget->initPlayers(playerNames); // если реализовано
    QList<Player> players;
        for (const QString& name : playerNames) {
            players.append(Player(name));
        }
    gameLogic = new GameLogic(this);
    gameLogic->startGame(players, filePath);

    gameWidget->setLogic(gameLogic);
    stackedWidget->setCurrentWidget(gameWidget);
}

void MainWindow::toMenu(){
    stackedWidget->setCurrentWidget(menuWidget);
}
void MainWindow::toPlayerSetup(){
    stackedWidget->setCurrentWidget(setupSinglePlayWidget);
}

void MainWindow::showLobby() {
    stackedWidget->setCurrentWidget(lobbyWindow);
}
MainWindow::~MainWindow()
{
    delete ui;
}

