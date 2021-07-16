#include "tp_playlist.h"
#include "ui_tp_playlist.h"

#include <QMouseEvent>

TP_Playlist::TP_Playlist(QWidget *parent) :
    QWidget(parent)
  , ui(new Ui::TP_Playlist)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    ui->pushButton_Close->setIcon(QIcon{":/image/icon_Exit.svg"});
}

TP_Playlist::~TP_Playlist()
{
    delete ui;
}

// *****************************************************************
// private slots:
// *****************************************************************

void TP_Playlist::on_pushButton_Close_clicked()
{
    hide();
}
