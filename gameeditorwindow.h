#ifndef GAMEEDITORWINDOW_H
#define GAMEEDITORWINDOW_H

#include <QMainWindow>
#include <QGroupBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class GameEditorWindow; }
QT_END_NAMESPACE

class GameEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    GameEditorWindow(QWidget *parent = nullptr);
    ~GameEditorWindow();

private slots:
    void on_saveButton_clicked();
    void on_createButton_clicked();

private:
    Ui::GameEditorWindow *ui;

    QJsonObject collectGameData();
    QJsonObject collectRoundData(QGroupBox* roundBox);
    QJsonObject collectThemeData(QGroupBox* themeBox);
    QJsonArray collectQuestions(QTableWidget* table);
};
#endif // GAMEEDITORWINDOW_H
