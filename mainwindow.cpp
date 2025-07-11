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

    //add pages in widgets
    stackedWidget->addWidget(menuWidget);
    stackedWidget->addWidget(setupSinglePlayWidget);
    stackedWidget->addWidget(gameWidget);

    connect(menuWidget, &MainMenuWidget::startSingleGameRequested, this, &MainWindow::showPlayerSetup);
    connect(menuWidget, &MainMenuWidget::editQuestionRequested, this, &MainWindow::showEditQuestionPack);
    connect(menuWidget, &MainMenuWidget::exitRequested, this, &MainWindow::close);

    connect(setupSinglePlayWidget, &PlayerSetupWidget::playersReady, this, &MainWindow::startGame);
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

MainWindow::~MainWindow()
{
    delete ui;
}

