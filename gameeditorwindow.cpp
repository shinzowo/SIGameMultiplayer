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
    // Очищаем и ставим одну «шаблонную» кнопку
    QLayoutItem* it;
    while ((it = ui->roundsLayout->takeAt(0)) != nullptr) {
        delete it->widget();
        delete it;
    }


    // создаём 3 обычных раунда и один финальный
    for (int i = 1; i <= 3; ++i) addRound();
    addRound(nullptr)->setTitle("Финал");
}

void GameEditorWindow::on_addRoundButton_clicked()
{
    addRound();
}

QGroupBox* GameEditorWindow::addRound(QGroupBox* /*after*/)
{
    static int cnt = 1;
    QGroupBox* roundBox = new QGroupBox(QString("Раунд %1").arg(cnt++), this);
    roundBox->setObjectName("roundGroupBox");

    auto* L = new QVBoxLayout(roundBox);
    // — заголовок раунда —
    auto* titleL = new QHBoxLayout;
    titleL->addWidget(new QLabel("Название раунда:"));
    QLineEdit* edt = new QLineEdit; edt->setObjectName("roundTitleEdit");
    titleL->addWidget(edt);
    L->addLayout(titleL);

    // — контейнер тем —
    QWidget* themesContainer = new QWidget; themesContainer->setObjectName("themesContainer");
    themesContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    L->addWidget(themesContainer);

    addTheme(roundBox);

    // === Кнопки раунда ===
    auto* btnL = new QHBoxLayout;
    QPushButton* btnAddR = new QPushButton("+ Раунд");
    QPushButton* btnRemR = new QPushButton("– Раунд");
    btnL->addStretch();
    btnL->addWidget(btnAddR);
    btnL->addWidget(btnRemR);
    L->addLayout(btnL);

    // привязки
    connect(btnAddR, &QPushButton::clicked, this, [=](){ addRound(roundBox); });
    connect(btnRemR, &QPushButton::clicked, this, [=](){ roundBox->deleteLater(); });


    // вставка в основной layout
    int pos = ui->roundsLayout->indexOf(ui->addRoundButton);
    ui->roundsLayout->insertWidget(pos, roundBox, /*stretch=*/1);

    return roundBox;
}

void GameEditorWindow::addTheme(QGroupBox* roundBox)
{
    // найдём контейнер и его layout
    auto* themesContainer = roundBox->findChild<QWidget*>("themesContainer");
    auto* themesL = qobject_cast<QVBoxLayout*>(themesContainer->layout());
    if (!themesL) {
        themesL = new QVBoxLayout(themesContainer);
        themesContainer->setLayout(themesL);
    }
    // создаём тему
    QGroupBox* themeBox = new QGroupBox("Тема", roundBox);
    themeBox->setObjectName("themeGroupBox");
    themeBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* thL = new QVBoxLayout(themeBox);
    // — название темы —
    auto* thTitleL = new QHBoxLayout;
    thTitleL->addWidget(new QLabel("Название темы:"));
    QLineEdit* thEdit = new QLineEdit; thEdit->setObjectName("themeTitleEdit");
    thTitleL->addWidget(thEdit);
    thL->addLayout(thTitleL);

    // — таблица вопросов и кнопки —
    QTableWidget* tbl = new QTableWidget(0,4);
    tbl->setHorizontalHeaderLabels({"Стоимость","Вопрос","Ответ","Тип"});
    tbl->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tbl->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    thL->addWidget(tbl, /*stretch=*/1);

    auto* qBtnL = new QHBoxLayout;
    QPushButton* btnAddQ = new QPushButton("+ Вопрос");
    QPushButton* btnRemQ = new QPushButton("– Вопрос");
    qBtnL->addWidget(btnAddQ);
    qBtnL->addWidget(btnRemQ);
    thL->addLayout(qBtnL);

    connect(btnAddQ, &QPushButton::clicked, this, [=](){ addQuestion(tbl); });
    connect(btnRemQ, &QPushButton::clicked, this, [=](){
        if (tbl->rowCount()) tbl->removeRow(tbl->rowCount()-1);
    });

    // — кнопки добавить/удалить тему —
    auto* thBtnL = new QHBoxLayout;
    QPushButton* btnAddTh = new QPushButton("+ Тема");
    QPushButton* btnRemTh = new QPushButton("– Тема");
    thBtnL->addWidget(btnAddTh);
    thBtnL->addStretch();
    thBtnL->addWidget(btnRemTh);
    thL->addLayout(thBtnL);

    connect(btnAddTh, &QPushButton::clicked, this, [=](){ addTheme(roundBox); });
    connect(btnRemTh, &QPushButton::clicked, this, [=](){ themeBox->deleteLater(); });

    themesL->addWidget(themeBox, /*stretch=*/1);
}

void GameEditorWindow::addQuestion(QTableWidget* table)
{
    table->insertRow(table->rowCount());
}


//                                             === Сбор данных для пака вопросов ===
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
