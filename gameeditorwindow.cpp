#include "GameEditorWindow.h"
#include "ui_GameEditorWindow.h"

#include <QJsonDocument>
#include <QFileDialog>
#include <QFile>
#include <QLineEdit>
#include <QMessageBox>

GameEditorWindow::GameEditorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::GameEditorWindow)
{
    ui->setupUi(this);
    setWindowTitle("Редактор SIGame");
}

GameEditorWindow::~GameEditorWindow()
{
    delete ui;
}
//Сохранение файла
void GameEditorWindow::on_saveButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить игру", "", "JSON (*.json)");
    if (fileName.isEmpty()) return;

    QJsonObject gameData = collectGameData();
    QJsonDocument doc(gameData);

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи.");
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}

void GameEditorWindow::on_createButton_clicked()
{
    // Очистка
    ui->gameTitleEdit->setText("Новая игра");

    QLayoutItem* item;
    while ((item = ui->roundsLayout->takeAt(0)) != nullptr) {
        if (item->widget())
            delete item->widget();
        delete item;
    }

    // Добавление 3 раунда
    for (int i = 1; i <= 3; ++i) {
        QString roundName = QString("Раунд %1").arg(i);
        QGroupBox* roundBox = new QGroupBox(roundName, this);
        QVBoxLayout* roundLayout = new QVBoxLayout(roundBox);

        // Добавим одну тему
        QGroupBox* themeBox = new QGroupBox("Тема", roundBox);
        QVBoxLayout* themeLayout = new QVBoxLayout(themeBox);

        QHBoxLayout* themeTitleLayout = new QHBoxLayout;
        QLabel* themeLabel = new QLabel("Название темы:");
        QLineEdit* themeTitleEdit = new QLineEdit;
        themeTitleEdit->setObjectName("themeTitleEdit");
        themeTitleLayout->addWidget(themeLabel);
        themeTitleLayout->addWidget(themeTitleEdit);

        QTableWidget* questionsTable = new QTableWidget(5, 4);
        questionsTable->setHorizontalHeaderLabels(QStringList() << "Стоимость" << "Вопрос" << "Ответ" << "Тип");
        questionsTable->setObjectName("questionsTable");

        themeLayout->addLayout(themeTitleLayout);
        themeLayout->addWidget(questionsTable);
        themeBox->setLayout(themeLayout);

        roundLayout->addWidget(themeBox);
        roundBox->setLayout(roundLayout);

        ui->roundsLayout->insertWidget(ui->roundsLayout->count() - 1, roundBox);
    }

    // Финальный раунд
    QGroupBox* finalBox = new QGroupBox("Финал", this);
    QVBoxLayout* finalLayout = new QVBoxLayout(finalBox);

    QGroupBox* finalThemeBox = new QGroupBox("Финальный вопрос", finalBox);
    QVBoxLayout* finalThemeLayout = new QVBoxLayout(finalThemeBox);

    QHBoxLayout* finalTitleLayout = new QHBoxLayout;
    QLabel* finalLabel = new QLabel("Название темы:");
    QLineEdit* finalTitleEdit = new QLineEdit;
    finalTitleEdit->setObjectName("themeTitleEdit");
    finalTitleLayout->addWidget(finalLabel);
    finalTitleLayout->addWidget(finalTitleEdit);

    QTableWidget* finalTable = new QTableWidget(1, 4);
    finalTable->setHorizontalHeaderLabels(QStringList() << "Стоимость" << "Вопрос" << "Ответ" << "Тип");
    finalTable->setObjectName("questionsTable");

    finalThemeLayout->addLayout(finalTitleLayout);
    finalThemeLayout->addWidget(finalTable);
    finalThemeBox->setLayout(finalThemeLayout);

    finalLayout->addWidget(finalThemeBox);
    finalBox->setLayout(finalLayout);

    ui->roundsLayout->insertWidget(ui->roundsLayout->count() - 1, finalBox);
}

QJsonObject GameEditorWindow::collectGameData()
{
    QJsonObject gameObject;
    gameObject["title"] = ui->gameTitleEdit->text();

    QJsonArray roundsArray;
    const auto roundWidgets = ui->scrollContent->findChildren<QGroupBox*>("roundGroupBox", Qt::FindChildrenRecursively);
    for (auto* roundBox : roundWidgets) {
        roundsArray.append(collectRoundData(roundBox));
    }

    gameObject["rounds"] = roundsArray;
    return gameObject;
}

QJsonObject GameEditorWindow::collectRoundData(QGroupBox* roundBox)
{
    QJsonObject roundObject;
    roundObject["name"] = roundBox->title();

    QJsonArray themesArray;
    const auto themeWidgets = roundBox->findChildren<QGroupBox*>("themeGroupBox", Qt::FindChildrenRecursively);
    for (auto* themeBox : themeWidgets) {
        themesArray.append(collectThemeData(themeBox));
    }

    roundObject["themes"] = themesArray;
    return roundObject;
}

QJsonObject GameEditorWindow::collectThemeData(QGroupBox* themeBox)
{
    QJsonObject themeObject;
    QLineEdit* titleEdit = themeBox->findChild<QLineEdit*>("themeTitleEdit");
    QTableWidget* table = themeBox->findChild<QTableWidget*>("questionsTable");

    themeObject["name"] = titleEdit ? titleEdit->text() : "Без названия";
    themeObject["questions"] = collectQuestions(table);

    return themeObject;
}

QJsonArray GameEditorWindow::collectQuestions(QTableWidget* table)
{
    QJsonArray questions;

    if (!table) return questions;

    for (int row = 0; row < table->rowCount(); ++row) {
        QJsonObject question;
        QString cost = table->item(row, 0) ? table->item(row, 0)->text() : "";
        QString text = table->item(row, 1) ? table->item(row, 1)->text() : "";
        QString answer = table->item(row, 2) ? table->item(row, 2)->text() : "";
        QString type = table->item(row, 3) ? table->item(row, 3)->text() : "";

        if (text.isEmpty()) continue;

        question["cost"] = cost.toInt();
        question["text"] = text;
        question["answer"] = answer;
        question["type"] = type;

        questions.append(question);
    }

    return questions;
}
