#include "tp_playlistwindow.h"
#include "ui_tp_playlistwindow.h"

#include "tp_globalconst.h"

#include <QMouseEvent>
#include <QStandardItemModel>

#include <filesystem>

TP_PlaylistWindow::TP_PlaylistWindow(QWidget *parent) :
    QWidget(parent)
  , ui( new Ui::TP_PlaylistWindow )
  , model_ListOfPlaylists( new QStandardItemModel{this} )
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    ui->pushButton_Close->setIcon(QIcon{":/image/icon_Exit.svg"});

    initializePlaylist();
}

TP_PlaylistWindow::~TP_PlaylistWindow()
{
    delete ui;

    storePlaylist();
}

// *****************************************************************
// private slots
// *****************************************************************

void
TP_PlaylistWindow::on_pushButton_Close_clicked()
{
    hide();
}


// *****************************************************************
// private
// *****************************************************************

void
TP_PlaylistWindow::initializePlaylist()
{
    if( std::filesystem::exists(TP::playlistFilePath) )
    {
        qDebug() << "Existing playlist " << QString::fromStdString(TP::playlistFilePath) << " found.";

    }
    else
    {
        qDebug() << "Existing playlist not found.";
        ui->playlistsWidget->addItem(tr("Default"));
        for (int i {} ; i < 5;  i++)
            ui->playlistsWidget->addItem(QString("L%1").arg(i));
    }

    ui->playlistsWidget->setDragDropMode(QAbstractItemView::InternalMove);
}

void
TP_PlaylistWindow::storePlaylist()
{
    if( !std::filesystem::exists(TP::configDirectoryPath) )
        std::filesystem::create_directory(TP::configDirectoryPath);
}
