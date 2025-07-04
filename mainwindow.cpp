#include "mainwindow.h"
#include <QPalette>
#include "./ui_mainwindow.h"

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

    //add pages in widgets
    stackedWidget->addWidget(menuWidget);

    connect(menuWidget, &MainMenuWidget::editQuestionRequested, this, &MainWindow::showEditQuestionPack);
    connect(menuWidget, &MainMenuWidget::exitRequested, this, &MainWindow::close);
}

void MainWindow::showEditQuestionPack(){

}
MainWindow::~MainWindow()
{
    delete ui;
}

