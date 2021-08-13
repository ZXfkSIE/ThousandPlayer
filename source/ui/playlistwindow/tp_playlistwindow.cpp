#include "tp_playlistwindow.h"
#include "ui_tp_playlistwindow.h"

#include "tp_globalconst.h"

#include "tp_filelistwidget.h"
#include "tp_menu.h"

// Headers of TagLib
#include <fileref.h>
#include <tag.h>

#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>

#include <filesystem>

//#include <cmath>

TP_PlaylistWindow::TP_PlaylistWindow(QWidget *parent) :
    QWidget { parent }
  , ui { new Ui::TP_PlaylistWindow }
{
    ui->setupUi(this);
    // Qt::Tool is used for getting rid of the window tab in taskbar
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);

    layout_FileListFrame = new QHBoxLayout { ui->frame_FileList };
    layout_FileListFrame->setContentsMargins(0, 0, 0, 0);

    ui->pushButton_Close->setIcon(QIcon{":/image/icon_Exit.svg"});

    initializeMenu();
    connectCurrentFileListWidget();
}

TP_PlaylistWindow::~TP_PlaylistWindow()
{
    delete ui;

    storePlaylist();
}

// Need to be executed manually after Main Class initialized the connections.
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

        currentFileListWidget = new TP_FileListWidget{ ui->frame_FileList, tr("Default") };
        vector_FileListWidget.push_back(currentFileListWidget);
        layout_FileListFrame->addWidget(currentFileListWidget);

        qDebug() << "emit signal_NewFilelistWidgetCreated(currentFileListWidget);";
        emit signal_NewFilelistWidgetCreated(currentFileListWidget);
    }
}

// *****************************************************************
// private slots
// *****************************************************************

void
TP_PlaylistWindow::slot_refreshAllShowingTitle()
{
    refreshShowingTitle(0, currentFileListWidget->count() - 1);
}

void
TP_PlaylistWindow::on_pushButton_Close_clicked()
{
    hide();
}

void TP_PlaylistWindow::on_action_AddFile_triggered()
{
    int originalCount { currentFileListWidget->count() };

    QStringList qstrlst_FilePath = QFileDialog::getOpenFileNames(
                this,                               // QWidget *parent = nullptr
                tr("Open files"),                   // const QString &caption = QString()
                QString(),                          // const QString &dir = QString()
                QString("FLAC files (*.flac)")      // const QString &filter = QString(),
                );

    for (QString& qstr_FilePath: qstrlst_FilePath)
    {
        QFileInfo fileInfo { QFile { qstr_FilePath } };
        QString qstr_Filename = fileInfo.fileName();

        TagLib::FileRef fileRef { qstr_FilePath.toLocal8Bit().constData() };
        QString qstr_title = TStringToQString(fileRef.tag()->title());
        QString qstr_artist = TStringToQString(fileRef.tag()->artist());
        QString qstr_album = TStringToQString(fileRef.tag()->album());

        int duration = fileRef.audioProperties()->lengthInSeconds();

        QListWidgetItem *item = new QListWidgetItem {currentFileListWidget};

        // set duration
        item->setData(TP::role_Duration,
                     QString("%1:%2")
                     .arg(duration / 60, 2, 10, QChar('0'))
                     .arg(duration % 60, 2, 10, QChar('0')));

        // set path
        item->setData(TP::role_Path, qstr_FilePath );

        // set filename
        item->setData(TP::role_Filename, qstr_Filename);

        //set descrption, artist, title, album
        if(qstr_title.length() == 0)
        {
            // No title in tag, meaning that no valid tag is contained in the file
            item->setData(TP::role_Description, qstr_Filename);
        }
        else
        {
            item->setData(TP::role_Description, qstr_artist + QString(" - ") + qstr_title);    // will be able to customized in the future
            item->setData(TP::role_Artist, qstr_artist);
            item->setData(TP::role_Title, qstr_title);
            item->setData(TP::role_Album, qstr_album);
        }
        currentFileListWidget->addItem(item);
    }

    refreshShowingTitle(originalCount - 1, currentFileListWidget->count() - 1 );
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_PlaylistWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    emit signal_Shown();
}

void
TP_PlaylistWindow::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    emit signal_Hidden();
}

// *****************************************************************
// private
// *****************************************************************

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
TP_PlaylistWindow::connectCurrentFileListWidget()
{
    if(currentFileListWidget != nullptr)
    {
        connect(currentFileListWidget, &TP_FileListWidget::signal_dropped, this, &TP_PlaylistWindow::slot_refreshAllShowingTitle);
    }
}

void
TP_PlaylistWindow::storePlaylist()
{
    if( !std::filesystem::exists(TP::configDirectoryPath) )
        std::filesystem::create_directory(TP::configDirectoryPath);
}

void
TP_PlaylistWindow::refreshShowingTitle( int idx_Min, int idx_Max )
{
    if ( idx_Max < 0 )
        return;

    if ( idx_Min == -1 )
        idx_Min++;

    for ( int i {idx_Min}; i <= idx_Max; i++ )
        currentFileListWidget->item(i)->setText(
                    QString("%1. ").arg(i + 1)
                    +
                    qvariant_cast<QString>(currentFileListWidget->item(i)->data(TP::role_Description)));
}
