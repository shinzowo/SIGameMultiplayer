#include "playerSetup.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QScrollArea>

PlayerSetupWidget::PlayerSetupWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    QFont font("Inter", 16);
    QString style = R"(
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

    QLabel *label = new QLabel("Введите имена игроков:");
    label->setFont(font);
    mainLayout->addWidget(label);

    // Секция для игроков
    playersLayout = new QVBoxLayout();
    mainLayout->addLayout(playersLayout);

    // Добавляем первого игрока по умолчанию
    onAddPlayerClicked();

    addPlayerButton = new QPushButton("Добавить игрока");

    connect(addPlayerButton, &QPushButton::clicked, this, &PlayerSetupWidget::onAddPlayerClicked);

    // Кнопка выбора файла вопросов
    loadButton = new QPushButton("Загрузить пакет вопросов");
    startButton = new QPushButton("Начать игру");


    QList<QPushButton*> buttons={addPlayerButton, loadButton, startButton};

    for(auto* btn: buttons){
        btn->setFixedSize(QSize(270, 30));
        btn->setStyleSheet(style);
        btn->setFont(font);
        mainLayout->addWidget(btn, 0, Qt::AlignHCenter);
        mainLayout->addSpacing(20);

        if (btn == loadButton) {
            fileLabel = new QLabel("Файл не выбран");
            fileLabel->setFont(font);
            mainLayout->addWidget(fileLabel, 0, Qt::AlignHCenter);
            mainLayout->addSpacing(20);
        }
    }
    connect(loadButton, &QPushButton::clicked, this, &PlayerSetupWidget::onLoadQuestionsClicked);
    connect(startButton, &QPushButton::clicked, this, &PlayerSetupWidget::onStartClicked);
}

void PlayerSetupWidget::onAddPlayerClicked() {
    QHBoxLayout *playerRowLayout = new QHBoxLayout;

    QLineEdit *playerNameEdit = new QLineEdit;
    playerNameEdit->setPlaceholderText("Имя игрока");
    playerNameEdit->setFont(QFont("Inter", 16));
    playerNameEdit->setMinimumWidth(200);
    playerEdits.append(playerNameEdit);

    QPushButton *removeButton = new QPushButton("Удалить");
    removeButton->setFont(QFont("Inter", 16));
    removeButton->setStyleSheet(R"(
        QPushButton {
            background-color: #e57373;
            color: white;
            border: 1px solid #000000;
            border-radius: 10px;
        }
        QPushButton:hover {
            background-color: #d32f2f;
        }
    )");
    removeButton->setFixedSize(QSize(100, 30));
    playerRowLayout->addWidget(playerNameEdit);
    playerRowLayout->addWidget(removeButton);

    QWidget *playerRowWidget = new QWidget;
    playerRowWidget->setLayout(playerRowLayout);

    playersLayout->addWidget(playerRowWidget);

    connect(removeButton, &QPushButton::clicked, this, [=]() {
        playersLayout->removeWidget(playerRowWidget);
        playerRowWidget->deleteLater();
    });
}


void PlayerSetupWidget::onLoadQuestionsClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите файл с вопросами", "", "JSON Files (*.json);;All Files (*)");
    if (!filePath.isEmpty()) {
        loadedFilePath = filePath;
        fileLabel->setText("Выбран файл: " + QFileInfo(filePath).fileName());
        emit questionsLoaded(filePath);
    }
}

void PlayerSetupWidget::onStartClicked() {
    QStringList names;
    for (QLineEdit *edit : playerEdits) {
        QString name = edit->text().trimmed();
        if (!name.isEmpty()) names << name;
    }

    if (names.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите хотя бы одного игрока.");
        return;
    }

    if (loadedFilePath.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Выберите пакет вопросов.");
        return;
    }

    emit playersReady(names, loadedFilePath);
}
