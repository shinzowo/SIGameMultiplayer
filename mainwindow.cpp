#include "mainwindow.h"
#include <QPalette>
#include "./ui_mainwindow.h"
#include "gameeditorwindow.h"
#include <QTimer>
#include <QDebug>
#include <QMessageBox>


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

    connect(connectionSetup, &ConnectionWidget::startServer, this, &MainWindow::onCreateServerClicked);
    connect(connectionSetup, &ConnectionWidget::connectToServer, this, &MainWindow::onConnectClicked);
    connect(connectionSetup, &ConnectionWidget::backToMenu, this, &MainWindow::toMenu);

    connect(lobbyWindow, &LobbyWindow::exitRequested, this, &MainWindow::showConnectionSetup);


    connect(gameWidget, &GameWidget::onBackClicked, this, &MainWindow::toPlayerSetup);
}
void MainWindow::showConnectionSetup()
{
    // 1. Отключаем клиента
    if (client) {
        client->deleteLater();     // автоматически вызывает disconnectFromHost()
        client = nullptr;
        qDebug() << "Client disconnected and deleted.";
    }

    // 2. Останавливаем сервер, если он был запущен
    if (server) {
        server->close();           // останавливает прослушивание
        server->deleteLater();
        server = nullptr;
        qDebug() << "Server stopped and deleted.";
    }

    // 3. Показываем экран подключения
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

// MainWindow::onCreateServerClicked()
void MainWindow::onCreateServerClicked()
{
    quint16 port = connectionSetup->getPort(); // метод получения порта
    qDebug() << "Port from connectionSetup: " << port;
    QString nickname = menuWidget->getNickname();
    // создаём сервер
    bool started=server = new GameServer(this);
    if (!started) {
        QMessageBox::critical(this, "Ошибка", "Не удалось запустить сервер на порту " + QString::number(port));
        return;
    }
    server->hostNickname = nickname;
    server->startServer(port);


    // создаём клиент даже для хоста (чтобы обновлять лобби так же)
    QTimer::singleShot(100, this, [=]() {
        client = new GameClient(this);
        connect(client, &GameClient::lobbyUpdated, this, &MainWindow::onLobbyUpdated);
        client->connectToServer("127.0.0.1", port, nickname);
    });

    showLobby();
}

// MainWindow::onConnectClicked()
void MainWindow::onConnectClicked()
{
    QString ip = connectionSetup->getIP();
    quint16 port = connectionSetup->getPort();
    QString nickname = menuWidget->getNickname();
    qDebug()<<"onConnectClicked"<<ip<<" "<<port<<" "<<nickname;
    client = new GameClient(this);
    connect(client, &GameClient::lobbyUpdated, this, &MainWindow::onLobbyUpdated);
    client->connectToServer(ip, port, nickname);

    showLobby();
}

void MainWindow::onLobbyUpdated(const QStringList &players)
{
    lobbyWindow->ui->HostNickname->clear();
    lobbyWindow->ui->Player1->clear();
    lobbyWindow->ui->Player2->clear();
    lobbyWindow->ui->Player3->clear();
    lobbyWindow->ui->Player4->clear();

    for (const QString &entry : players) {
        if (entry.startsWith("Host:"))
            lobbyWindow->ui->HostNickname->setText(entry.section(':', 1).trimmed());
        else if (entry.startsWith("Player1:"))
            lobbyWindow->ui->Player1->setText(entry.section(':', 1).trimmed());
        else if (entry.startsWith("Player2:"))
            lobbyWindow->ui->Player2->setText(entry.section(':', 1).trimmed());
        else if (entry.startsWith("Player3:"))
            lobbyWindow->ui->Player3->setText(entry.section(':', 1).trimmed());
        else if (entry.startsWith("Player4:"))
            lobbyWindow->ui->Player4->setText(entry.section(':', 1).trimmed());
    }
}




