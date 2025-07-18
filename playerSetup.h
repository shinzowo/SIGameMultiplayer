#ifndef PLAYERSETUP_H
#define PLAYERSETUP_H
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVector>

class PlayerSetupWidget : public QWidget {
    Q_OBJECT

public:
    explicit PlayerSetupWidget(QWidget *parent = nullptr);

signals:
    void playersReady(const QStringList &names, const QString &loadedFilePath);
    void questionsLoaded(const QString &filePath);
    void onBackClicked();

private slots:
    void onAddPlayerClicked();
    void onStartClicked();
    void onLoadQuestionsClicked();

private:
    QVBoxLayout *playersLayout;
    QVector<QLineEdit*> playerEdits;
    QPushButton *addPlayerButton;
    QPushButton *startButton;
    QPushButton *loadButton;
    QPushButton *backButton;
    QLabel *fileLabel;
    QString loadedFilePath;

};
#endif // PLAYERSETUP_H

