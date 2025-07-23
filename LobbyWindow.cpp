#include "LobbyWindow.h"
#include "ui_LobbyWindow.h"

LobbyWindow::LobbyWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LobbyWindow)
{
    ui->setupUi(this);
}

LobbyWindow::~LobbyWindow() {
    delete ui;
}
