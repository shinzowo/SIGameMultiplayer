#include "menu.h"
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QPixmap>
#include <QPropertyAnimation>
#include <QTimer>
#include <QParallelAnimationGroup>


QGraphicsDropShadowEffect* createShadowEffect();

MainMenuWidget::MainMenuWidget(QWidget *parent):QWidget(parent){
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);


    QHBoxLayout *topLayout=new QHBoxLayout();
    topLayout->setContentsMargins(10, 10, 10, 0); // отступ сверху и слева
    topLayout->setSpacing(0);

    QLabel *logoLabel=new QLabel();
    QPixmap logo(":res/Logo.png");
    logoLabel->setPixmap(logo);
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);


    QPushButton *playMPBtn = new QPushButton("ИГРА ПО СЕТИ");
    QPushButton *hotseatBtn = new QPushButton("ХОТСИТ");
    QPushButton *editQuestionBtn = new QPushButton("РЕДАКТОР ВОПРОСОВ");
    QPushButton *exitBtn = new QPushButton("ВЫХОД");

    QList<QPushButton*> buttons={playMPBtn, hotseatBtn, editQuestionBtn, exitBtn};
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

    // Обертка для иконки и панели
    QHBoxLayout* profileLayout = new QHBoxLayout();
    profileLayout->setSpacing(10);

    // Редактирование иконки профиля
    QPushButton* profileBtn = new QPushButton();
    QPixmap iconPm(":res/ProfileIcon.png");
    QIcon ProfileIcon(iconPm);
    profileBtn->setIcon(ProfileIcon);
    profileBtn->setIconSize(QSize(75,75));
    profileBtn->setFlat(true);
    profileBtn->setCursor(Qt::PointingHandCursor);

    // Панель профиля
    QWidget* profilePanel = new QWidget();
    profilePanel->setMaximumWidth(0); // Скрыта по умолчанию
    profilePanel->setStyleSheet("background-color: #42A5F5; border-radius: 5px;");
    QVBoxLayout* panelLayout = new QVBoxLayout(profilePanel);
    QLabel* profileLabel = new QLabel("Профиль");
    profileLabel->setStyleSheet("color: white;");
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Введите имя");
    panelLayout->addWidget(profileLabel);
    panelLayout->addWidget(nameEdit);

    // В одну строку
    profileLayout->addWidget(profileBtn);
    profileLayout->addWidget(profilePanel);
    profileLayout->addStretch();
    profileLayout->setAlignment(Qt::AlignTop);

    // Добавляем в topLayout
    topLayout->addLayout(profileLayout);
    topLayout->addStretch();  // справа отводим место

    layout->addLayout(topLayout);

    layout->setAlignment(topLayout, Qt::AlignTop);
    layout->addWidget(logoLabel, 0, Qt::AlignHCenter);
    layout->addStretch();

    for (auto *btn : buttons) {
        btn->setMinimumWidth(264);  // вместо setFixedSize
        btn->setMaximumHeight(0);   // для анимации высоты
        btn->setStyleSheet(style);
        btn->setFont(font);

        btn->setGraphicsEffect(new QGraphicsOpacityEffect(btn));  // прозрачность
        static_cast<QGraphicsOpacityEffect*>(btn->graphicsEffect())->setOpacity(0.0);

        layout->addWidget(btn, 0, Qt::AlignHCenter);
        layout->addSpacing(20);
    }

    // Анимация с задержками
    for (int i = 0; i < buttons.size(); ++i) {
        QTimer::singleShot(i * 150, [=]() {
            QPushButton* btn = buttons[i];
            auto *opacity = qobject_cast<QGraphicsOpacityEffect*>(btn->graphicsEffect());

            // Анимация высоты
            auto *heightAnim = new QPropertyAnimation(btn, "maximumHeight");
            heightAnim->setDuration(400);
            heightAnim->setStartValue(0);
            heightAnim->setEndValue(35);

            // Анимация прозрачности
            auto *fadeAnim = new QPropertyAnimation(opacity, "opacity");
            fadeAnim->setDuration(400);
            fadeAnim->setStartValue(0.0);
            fadeAnim->setEndValue(1.0);

            // Групповая анимация
            auto *group = new QParallelAnimationGroup(btn);
            group->addAnimation(heightAnim);
            group->addAnimation(fadeAnim);
            group->start(QAbstractAnimation::DeleteWhenStopped);
        });
    }
    layout->addStretch();

    bool isExpanded = false;
    connect(profileBtn, &QPushButton::clicked, this, [=]() mutable {
        auto *anim = new QPropertyAnimation(profilePanel, "maximumWidth");
        anim->setDuration(300);
        anim->setStartValue(isExpanded ? 200 : 0);
        anim->setEndValue(isExpanded ? 0 : 200);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->start(QAbstractAnimation::DeleteWhenStopped);
        isExpanded = !isExpanded;
    });

    connect(playMPBtn, &QPushButton::clicked, this, &MainMenuWidget::startMultiplayerGameRequested);
    connect(hotseatBtn, &QPushButton::clicked, this, &MainMenuWidget::startSingleGameRequested);
    connect(editQuestionBtn, &QPushButton::clicked, this, &MainMenuWidget::editQuestionRequested);
    connect(exitBtn, &QPushButton::clicked, this, &MainMenuWidget::exitRequested);

    connect(profileBtn, &QPushButton::clicked, this, &MainMenuWidget::saveNickname);
}

QGraphicsDropShadowEffect* createShadowEffect() {
    auto* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(10);
    shadow->setOffset(2, 2);
    shadow->setColor(QColor(0, 0, 0, 100));
    return shadow;
}

QString MainMenuWidget::getNickname(){
    return nickname;
}

void MainMenuWidget::saveNickname(){
    bool isActuallyEmpty = nameEdit->text().isEmpty() ||
                          nameEdit->text() == nameEdit->placeholderText();
    if(!isActuallyEmpty){
        nickname=nameEdit->text();
    }
    qDebug()<<"Nickname:"<<nickname;
}
