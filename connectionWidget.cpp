#include "ConnectionWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QFileDialog>

ConnectionWidget::ConnectionWidget(QWidget *parent) : QWidget(parent) {
    QFont font("Inter", 16);
    QString buttonStyle = R"(
        QPushButton {
            background-color: #c9d052;
            color: black;
            border: 1px solid #000000;
            border-radius: 10px;
        }
        QPushButton:hover {
            background-color: #A8AE43;
        }
        QPushButton:pressed {
            background-color: #1f618d;
        }
    )";

    auto* mainLayout = new QVBoxLayout(this);

    modeSelector = new QComboBox();
    modeSelector->addItem("Создать сервер");
    modeSelector->addItem("Подключиться");
    modeSelector->setFont(font);
    mainLayout->addWidget(modeSelector);

    stackedWidget = new QStackedWidget();
    mainLayout->addWidget(stackedWidget);

    // --- Серверная страница ---
    serverPage = new QWidget();
    auto* serverLayout = new QVBoxLayout(serverPage);

    QLabel* serverTitle = new QLabel("Создание сервера");
    serverTitle->setFont(font);
    serverTitle->setAlignment(Qt::AlignCenter);
    serverLayout->addWidget(serverTitle);

    QLabel* serverPortLabel = new QLabel("Порт для подключения:");
    serverPortLabel->setFont(font);
    serverLayout->addWidget(serverPortLabel);

    serverPortEdit = new QLineEdit();
    serverPortEdit->setFont(font);
    serverPortEdit->setPlaceholderText("Например: 4242");
    serverLayout->addWidget(serverPortEdit);

    serverActionButton = new QPushButton("Создать сервер");
    serverActionButton->setFont(font);
    serverActionButton->setStyleSheet(buttonStyle);
    serverLayout->addWidget(serverActionButton);

    loadPackButton = new QPushButton("Загрузить пак");
    loadPackButton->setFont(font);
    loadPackButton->setStyleSheet(buttonStyle);
    serverLayout->addWidget(loadPackButton);
    connect(loadPackButton, &QPushButton::clicked, this, &ConnectionWidget::onLoadPackButtonClicked);

    stackedWidget->addWidget(serverPage);

    // --- Клиентская страница ---
    clientPage = new QWidget();
    auto* clientLayout = new QVBoxLayout(clientPage);

    QLabel* clientTitle = new QLabel("Подключение к серверу");
    clientTitle->setFont(font);
    clientTitle->setAlignment(Qt::AlignCenter);
    clientLayout->addWidget(clientTitle);

    QLabel* clientIpLabel = new QLabel("IP адрес сервера:");
    clientIpLabel->setFont(font);
    clientLayout->addWidget(clientIpLabel);

    clientIpEdit = new QLineEdit();
    clientIpEdit->setFont(font);
    clientIpEdit->setPlaceholderText("Например: 192.168.1.100");
    clientLayout->addWidget(clientIpEdit);

    QLabel* clientPortLabel = new QLabel("Порт для подключения:");
    clientPortLabel->setFont(font);
    clientLayout->addWidget(clientPortLabel);

    clientPortEdit = new QLineEdit();
    clientPortEdit->setFont(font);
    clientPortEdit->setPlaceholderText("Например: 4242");
    clientLayout->addWidget(clientPortEdit);

    clientActionButton = new QPushButton("Подключиться");
    clientActionButton->setFont(font);
    clientActionButton->setStyleSheet(buttonStyle);
    clientLayout->addWidget(clientActionButton);

    stackedWidget->addWidget(clientPage);

    backButton = new QPushButton("Назад");
    backButton->setFont(font);
    backButton->setStyleSheet(buttonStyle);
    mainLayout->addWidget(backButton);

    // Сигналы
    connect(modeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ConnectionWidget::modeChanged);
    connect(serverActionButton, &QPushButton::clicked, this, &ConnectionWidget::handleActionClicked);
    connect(clientActionButton, &QPushButton::clicked, this, &ConnectionWidget::handleActionClicked);
    connect(backButton, &QPushButton::clicked, this, &ConnectionWidget::backToMenu);

    modeChanged(0);
}

void ConnectionWidget::modeChanged(int index) {
    stackedWidget->setCurrentIndex(index);
}

void ConnectionWidget::handleActionClicked() {
    if (stackedWidget->currentWidget() == serverPage) {
        if(loadPackPath.isEmpty()){
            QMessageBox::information(this, "Сообщение", "Загрузите пак");
            return;
        }
        QString rawPort = serverPortEdit->text();
        qDebug() << "Raw port input:" << rawPort;

        QString trimmed = rawPort.trimmed();
        qDebug() << "Trimmed port input:" << trimmed;

        bool ok = false;
        quint16 parsedPort = trimmed.toUShort(&ok);

        if (!ok || parsedPort == 0) {
            qDebug() << "Invalid port entered.";
            return;
        }

        port = parsedPort; // ✅ Сохраняем!
        qDebug() << "Enter port (server): " << port;
        emit startServer(port);
    } else if (stackedWidget->currentWidget() == clientPage) {
        ip = clientIpEdit->text();
        bool ok = false;
        quint16 parsedPort = clientPortEdit->text().trimmed().toUShort(&ok);

        if (!ok || parsedPort == 0) {
            qDebug() << "Invalid port entered (client).";
            return;
        }

        port = parsedPort; // ✅ Тоже сохраняем!
        qDebug() << "Enter port (client): " << ip << port;
        emit connectToServer(ip, port);
    }
}




QString ConnectionWidget::getIP(){
    return ip;
}
quint16 ConnectionWidget::getPort(){
    return port;
}

QString ConnectionWidget::getLoadPath(){
    return loadPackPath;
}

void ConnectionWidget::onLoadPackButtonClicked() {
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Все файлы (*.*)");
    if (!fileName.isEmpty()) {
        loadPackPath = fileName;
        serverActionButton->setEnabled(true);
        qDebug() << "Выбранный файл:" << loadPackPath;

    }
}





