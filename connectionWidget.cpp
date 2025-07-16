#include "ConnectionWidget.h"
#include <QHBoxLayout>
#include <QFont>

ConnectionWidget::ConnectionWidget(QWidget *parent)
    : QWidget(parent)
{
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

    auto* layout = new QVBoxLayout(this);

    QLabel* titleLabel = new QLabel("Подключение к игре");
    titleLabel->setFont(font);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);

    QLabel* ipLabel = new QLabel("IP адрес:");
    ipLabel->setFont(font);
    ipEdit = new QLineEdit();
    ipEdit->setFont(font);
    ipEdit->setPlaceholderText("Например: 127.0.0.1");

    QLabel* portLabel = new QLabel("Порт:");
    portLabel->setFont(font);
    portEdit = new QLineEdit();
    portEdit->setFont(font);
    portEdit->setPlaceholderText("Например: 4242");

    layout->addWidget(ipLabel);
    layout->addWidget(ipEdit);
    layout->addWidget(portLabel);
    layout->addWidget(portEdit);

    connectButton = new QPushButton("Подключиться");
    connectButton->setFont(font);
    connectButton->setStyleSheet(buttonStyle);

    backButton = new QPushButton("Назад");
    backButton->setFont(font);
    backButton->setStyleSheet(buttonStyle);

    layout->addWidget(connectButton);
    layout->addWidget(backButton);

    connect(connectButton, &QPushButton::clicked, this, &ConnectionWidget::handleConnectClicked);
    connect(backButton, &QPushButton::clicked, this, &ConnectionWidget::backToMenu);
}

void ConnectionWidget::handleConnectClicked() {
    QString ip = ipEdit->text();
    quint16 port = portEdit->text().toUShort();
    emit connectToServer(ip, port);
}
