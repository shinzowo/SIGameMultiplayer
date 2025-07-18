#ifndef GAMEEDITORWINDOW_H
#define GAMEEDITORWINDOW_H

#include <QMainWindow>
#include <QGroupBox>
#include <QTableWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QHeaderView>

QT_BEGIN_NAMESPACE
namespace Ui {
class GameEditorWindow;
}
QT_END_NAMESPACE

class QTableWidget;

class GameEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameEditorWindow(QWidget *parent = nullptr);
    ~GameEditorWindow();

private slots:
    void on_saveButton_clicked();
    void on_createButton_clicked();
    void on_addRoundButton_clicked();

private:
    Ui::GameEditorWindow *ui;

    // Динамическое создание элементов
    QGroupBox* addRound(QGroupBox* after = nullptr);
    void       addTheme(QGroupBox* roundBox);
    void       addQuestion(QTableWidget* table);

    // Сбор данных в JSON
    QJsonObject collectGameData();
    QJsonObject collectRoundData(QGroupBox* roundBox);
    QJsonObject collectThemeData(QGroupBox* themeBox);
    QJsonArray  collectQuestions(QTableWidget* table);
};

#endif // GAMEEDITORWINDOW_H
