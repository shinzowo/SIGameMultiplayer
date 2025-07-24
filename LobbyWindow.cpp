#include "LobbyWindow.h"
#include "ui_LobbyWindow.h"
#include <QDebug>

LobbyWindow::LobbyWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LobbyWindow)
{
    ui->setupUi(this);

    connect(ui->exitButton, &QPushButton::clicked, this, &LobbyWindow::handleExitButton);
}

LobbyWindow::~LobbyWindow() {
    delete ui;
}

void LobbyWindow::handleExitButton()
{
    emit exitRequested();
}


