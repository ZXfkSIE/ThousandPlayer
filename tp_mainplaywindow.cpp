#include "tp_mainplaywindow.h"
#include "./ui_tp_mainplaywindow.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

TP_MainPlayWindow::TP_MainPlayWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TP_MainPlayWindow)
{
    ui->setupUi(this);
}

TP_MainPlayWindow::~TP_MainPlayWindow()
{
    delete ui;
}

void
TP_MainPlayWindow::show()
{
    QPropertyAnimation *fadeInAnimation = new QPropertyAnimation(this, "windowOpacity");
    fadeInAnimation->setDuration(2000);
    fadeInAnimation->setStartValue(0);
    fadeInAnimation->setEndValue(1);
    fadeInAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    QWidget::show();
}

// *****************************************************************
// private
// *****************************************************************
