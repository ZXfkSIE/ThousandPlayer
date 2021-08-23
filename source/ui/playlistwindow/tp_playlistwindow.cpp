#include "tp_playlistwindow.h"
#include "ui_tp_playlistwindow.h"

#include "tp_globalconst.h"

#include "tp_filelistwidget.h"
#include "tp_menu.h"

#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>

#include <filesystem>

// Headers of TagLib
#include <fileref.h>
#include <tag.h>

TP_PlaylistWindow::TP_PlaylistWindow(QWidget *parent) :
    QWidget { parent }
  , ui { new Ui::TP_PlaylistWindow }
  , currentFileListWidget { nullptr }
  , menu_Add { nullptr }
{
    ui->setupUi(this);
    // Qt::Tool is used for getting rid of the window tab in taskbar
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);

    layout_FileListFrame = new QHBoxLayout { ui->frame_FileList };
    layout_FileListFrame->setContentsMargins(0, 0, 0, 0);

    ui->pushButton_Close->setIcon(QIcon{":/image/icon_Exit.svg"});

    initializeMenu();

    /****************************** WARNING *****************************************
     *  The method initializePlaylist()
     *  need to be executed manually after TP_MainClass initialized the connections.
     ********************************************************************************/
}

TP_PlaylistWindow::~TP_PlaylistWindow()
{
    delete ui;

    storePlaylist();
}

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
        connectCurrentFileListWidget();
        vector_FileListWidget.push_back(currentFileListWidget);
        layout_FileListFrame->addWidget(currentFileListWidget);

        qDebug() << "[SIGNAL] signal_NewFilelistWidgetCreated -- list's name is " << currentFileListWidget->getListName();
        emit signal_NewFilelistWidgetCreated(currentFileListWidget);
    }
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
    int originalCount { currentFileListWidget->count() };

    QStringList qstrlst_FilePath = QFileDialog::getOpenFileNames(
                this,                               // QWidget *parent = nullptr
                tr("Open files"),                   // const QString &caption = QString()
                QString(),                          // const QString &dir = QString()
                QString("FLAC files (*.flac);;"     // const QString &filter = QString()
                        "MP3 files (*.mp3)")
                );

    for (QString& qstr_FilePath: qstrlst_FilePath)
    {
        QFileInfo fileInfo { QFile { qstr_FilePath } };
        QString qstr_Filename = fileInfo.fileName();
        QString qstr_extension = fileInfo.suffix().toLower();

        TagLib::FileRef fileRef { qstr_FilePath.toLocal8Bit().constData() };
        QString qstr_title = TStringToQString(fileRef.tag()->title());
        QString qstr_artist = TStringToQString(fileRef.tag()->artist());
        QString qstr_album = TStringToQString(fileRef.tag()->album());

        int duration = fileRef.audioProperties()->lengthInSeconds();

        QListWidgetItem *item = new QListWidgetItem {currentFileListWidget};

        // set duration
        item->setData(TP::role_Duration, duration);

        // set file path URL
        item->setData(TP::role_Path, qstr_FilePath );

        // set file name
        item->setData(TP::role_FileName, qstr_Filename);

        // set file type
        if( qstr_extension == QString("flac") )
            item->setData(TP::role_FileType, TP::FileFormat::FLAC);
        else if( qstr_extension == QString("mp3") )
            item->setData(TP::role_FileType, TP::FileFormat::MP3);

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

    emit signal_refreshShowingTitle( originalCount - 1, currentFileListWidget->count() - 1 );
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
TP_PlaylistWindow::disconnectCurrentFileListWidget()
{
    if(currentFileListWidget != nullptr)
        disconnect(this,                    &TP_PlaylistWindow::signal_refreshShowingTitle,
                   currentFileListWidget,   &TP_FileListWidget::slot_refreshShowingTitle);
}

void
TP_PlaylistWindow::connectCurrentFileListWidget()
{
    if(currentFileListWidget != nullptr)
        connect(this,                    &TP_PlaylistWindow::signal_refreshShowingTitle,
                currentFileListWidget,   &TP_FileListWidget::slot_refreshShowingTitle);
}

void
TP_PlaylistWindow::storePlaylist()
{
    if( !std::filesystem::exists(TP::configDirectoryPath) )
        std::filesystem::create_directory(TP::configDirectoryPath);
}
