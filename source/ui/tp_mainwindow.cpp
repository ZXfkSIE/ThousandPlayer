#include "tp_mainwindow.h"
#include "./ui_tp_mainwindow.h"

TP_MainWindow::TP_MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TP_MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    ui->pushButton_Minimize->setIcon(QIcon{":/image/icon_Minimize.svg"});
    ui->pushButton_Expand->setIcon(QIcon{":/image/icon_Expand.svg"});
    ui->pushButton_Exit->setIcon(QIcon{":/image/icon_Exit.svg"});
}

TP_MainWindow::~TP_MainWindow()
{
    delete ui;
}

// *****************************************************************
// private
// *****************************************************************

void TP_MainWindow::on_pushButton_Exit_clicked()
{
    close();
}

