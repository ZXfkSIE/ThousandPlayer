#include "tp_playlistwindow.h"
#include "ui_tp_playlistwindow.h"

#include "tp_globalconst.h"

#include "tp_filelisttablewidget.h"
#include "tp_menu.h"

#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>

#include <filesystem>

TP_PlaylistWindow::TP_PlaylistWindow(QWidget *parent) :
    QWidget(parent)
  , ui( new Ui::TP_PlaylistWindow )
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    layout_FileListFrame = new QHBoxLayout {ui->frame_FileList};
    layout_FileListFrame->setContentsMargins(0, 0, 0, 0);

    ui->pushButton_Close->setIcon(QIcon{":/image/icon_Exit.svg"});

    initializePlaylist();
    initializeMenu();
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

void TP_PlaylistWindow::on_action_AddFile_triggered()
{
    QStringList qstrlst_FilePath = QFileDialog::getOpenFileNames(
                this,                               // QWidget *parent = nullptr
                tr("Open files"),                   // const QString &caption = QString()
                QString(),                          // const QString &dir = QString()
                QString("FLAC files (*.flac)")      // const QString &filter = QString(),
                );

    for (QString& qstr_FilePath: qstrlst_FilePath)
    {
        currentFileListTableWidget->insertRow(currentFileListTableWidget->rowCount());

    }
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
        qDebug() << "Existing playlist not found. Creating default playlist and filelist.";
        ui->playlistsWidget->addItem(tr("Default"));
        ui->playlistsWidget->setCurrentRow(0);

        for(int i{}; i < 50; i++)
            ui->playlistsWidget->addItem(QString("L%1").arg(i));

        currentFileListTableWidget = new TP_FileListTableWidget{ ui->frame_FileList, tr("Default") };
        vector_FileListTableWidget.push_back(currentFileListTableWidget);
        layout_FileListFrame->addWidget(currentFileListTableWidget);
    }
}

void
TP_PlaylistWindow::initializeMenu()
{
    // Initialize menu of "Add" button
    menu_Add = new TP_Menu {ui->pushButton_Add};

    menu_Add->addAction(ui->action_AddFile);
    menu_Add->addAction(ui->action_AddFolder);
    menu_Add->addSeparator();
    menu_Add->addAction(ui->action_AddURL);

    ui->pushButton_Add->setMenu(menu_Add);
}

void
TP_PlaylistWindow::storePlaylist()
{
    if( !std::filesystem::exists(TP::configDirectoryPath) )
        std::filesystem::create_directory(TP::configDirectoryPath);
}
