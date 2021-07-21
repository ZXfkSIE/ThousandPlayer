#include "tp_playlistwindow.h"
#include "ui_tp_playlistwindow.h"

#include <QMouseEvent>

TP_PlaylistWindow::TP_PlaylistWindow(QWidget *parent) :
    QWidget(parent)
  , ui(new Ui::TP_PlaylistWindow)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    ui->pushButton_Close->setIcon(QIcon{":/image/icon_Exit.svg"});
}

TP_PlaylistWindow::~TP_PlaylistWindow()
{
    delete ui;
}

// *****************************************************************
// private slots:
// *****************************************************************

void
TP_PlaylistWindow::on_pushButton_Close_clicked()
{
    hide();
}
