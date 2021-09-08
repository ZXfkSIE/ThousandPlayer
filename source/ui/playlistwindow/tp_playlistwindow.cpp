#include "tp_playlistwindow.h"
#include "ui_tp_playlistwindow.h"

#include "tp_globalconst.h"
#include "tp_globalvariable.h"

#include "tp_filelistwidget.h"
#include "tp_menu.h"

#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>

#include <filesystem>

// Headers of TagLib
#include <fileref.h>
#include <tag.h>
#include <flacproperties.h>

TP_PlaylistWindow::TP_PlaylistWindow( QWidget *parent ) :
    QWidget { parent }
  , ui { new Ui::TP_PlaylistWindow }
  , currentFileListWidget { nullptr }
  , vector_FileListWidget {}
  , menu_Add { nullptr }
{
    ui->setupUi(this);
    // Qt::Tool is used for getting rid of the window tab in taskbar
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint | Qt::Tool | Qt::NoDropShadowWindowHint );

    initializeConnection();

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
    if( std::filesystem::exists( TP::playlistFilePath.toLocal8Bit().constData() ) )
    {
        qDebug() << "Existing playlist " << TP::playlistFilePath << " found.";
    }
    else
    {
        qDebug() << "Existing playlist not found. Creating default playlist and filelist.";
        ui->playlistsWidget->addItem(tr("Default"));
        ui->playlistsWidget->setCurrentRow(0);

        currentFileListWidget = new TP_FileListWidget{ ui->frame_FileList, tr("Default") };
        connectCurrentFileListWidget();
        vector_FileListWidget.push_back( currentFileListWidget );
        layout_FileListFrame->addWidget( currentFileListWidget );

        qDebug() << "[SIGNAL] signal_NewFilelistWidgetCreated -- list's name is " << currentFileListWidget->getListName();
        emit signal_newFilelistWidgetCreated( currentFileListWidget );
    }
}


void
TP_PlaylistWindow::setCurrentItem( QListWidgetItem * I_item )
{
    currentFileListWidget->_setCurrentItem( I_item );
}


void TP_PlaylistWindow::setCurrentItemBold()
{
    currentFileListWidget->setCurrentItemBold();
}


void TP_PlaylistWindow::unsetCurrentItemBold()
{
    currentFileListWidget->unsetCurrentItemBold();
}


QListWidgetItem *
TP_PlaylistWindow::getCurrentItem()
{
    return currentFileListWidget->getCurrentItem();
}


QListWidgetItem *
TP_PlaylistWindow::getNextItem()
{
    return currentFileListWidget->getNextItem();
}


QListWidgetItem *
TP_PlaylistWindow::getPreviousItem()
{
    return currentFileListWidget->getPreviousItem();
}


QListWidgetItem *
TP_PlaylistWindow::getNextItem_shuffle()
{
    return currentFileListWidget->getNextItem_shuffle();
}


QListWidgetItem *
TP_PlaylistWindow::getPreviousItem_shuffle()
{
    return currentFileListWidget->getPreviousItem_shuffle();
}


// *****************************************************************
// public slots
// *****************************************************************

void
TP_PlaylistWindow::slot_modeIsNotShuffle()
{
    currentFileListWidget->modeIsNotShuffle();
}


// *****************************************************************
// private slots
// *****************************************************************


void
TP_PlaylistWindow::slot_moveTitleBar( QRect newGeometry )
{
    emit signal_moveWindow( this, newGeometry );
}


void
TP_PlaylistWindow::slot_leftButtonReleased()
{
    emit signal_leftButtonReleased();
}


void
TP_PlaylistWindow::slot_resizeWindow( QRect newGeomtry, TP::ResizeType resizeType )
{
    emit signal_resizeWindow( this, newGeomtry, resizeType );
}


void
TP_PlaylistWindow::on_pushButton_Close_clicked()
{
    hide();
}


void TP_PlaylistWindow::on_action_addFile_triggered()
{
    int originalCount { currentFileListWidget->count() };

    QList <QUrl> qlist_FileURLs = QFileDialog::getOpenFileUrls(
                this,                               // QWidget *parent = nullptr
                tr("Open files"),                   // const QString &caption = QString()
                QString(),                          // const QString &dir = QString()
                QString("FLAC files (*.flac);;"     // const QString &filter = QString()
                        "MP3 files (*.mp3)")
                );

    for (const QUrl& fileURL: qlist_FileURLs)
    {

        QString qstr_localFilePath = fileURL.toLocalFile();
        QFileInfo fileInfo { QFile { qstr_localFilePath } };
        QString qstr_Filename = fileInfo.fileName();

        TagLib::FileRef fileRef { qstr_localFilePath.toLocal8Bit().constData() };
        QString qstr_title = TStringToQString( fileRef.tag()->title() );
        QString qstr_artist = TStringToQString( fileRef.tag()->artist() );
        QString qstr_album = TStringToQString( fileRef.tag()->album() );

        int duration = fileRef.audioProperties()->lengthInSeconds();
        int bitrate = fileRef.audioProperties()->bitrate();
        int sampleRate = fileRef.audioProperties()->sampleRate() / 1000;
        int bitDepth = -1 ;

        QString extension { QFileInfo { fileURL.toLocalFile() }.suffix().toLower() };
        if( extension == QString { "flac" } )
            bitDepth = dynamic_cast<TagLib::FLAC::Properties *>( fileRef.audioProperties() )->bitsPerSample();

        QListWidgetItem *item = new QListWidgetItem { currentFileListWidget };

        // set URL
        item->setData( TP::role_URL, fileURL );

        // set source type
        item->setData( TP::role_SourceType, TP::singleFile );

        // set duration
        item->setData( TP::role_Duration, duration );

        // set bitrate
        item->setData( TP::role_Bitrate, bitrate );

        // set sample rate
        item->setData( TP::role_SampleRate, sampleRate );

        // set bit depth
        item->setData( TP::role_BitDepth, bitDepth );

        // set file name
        item->setData(TP::role_FileName, qstr_Filename);

        //set descrption, artist, title, album
        if(qstr_title.length() == 0)
        {
            // No title in tag, meaning that no valid tag is contained in the file
            item->setData( TP::role_Description, qstr_Filename );
        }
        else
        {
            item->setData( TP::role_Description, qstr_artist + QString(" - ") + qstr_title );    // will be able to customized in the future
            item->setData( TP::role_Artist, qstr_artist );
            item->setData( TP::role_Title, qstr_title );
            item->setData( TP::role_Album, qstr_album );
        }
        currentFileListWidget->addItem( item );
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
    emit signal_shown();
}

void
TP_PlaylistWindow::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    emit signal_hidden();
}

// *****************************************************************
// private
// *****************************************************************

void
TP_PlaylistWindow::initializeMenu()
{
    // Initialize menu of "Add" button
    menu_Add = new TP_Menu { ui->pushButton_Add };

    menu_Add->addAction( ui->action_addFile );
    menu_Add->addAction( ui->action_addFolder );
    menu_Add->addSeparator();
    menu_Add->addAction( ui->action_addURL );

    ui->pushButton_Add->setMenu( menu_Add );
}

void
TP_PlaylistWindow::initializeConnection()
{
    connect(ui->frame_Title,        &TP_TitleBar::signal_moveTitleBar,
            this,                   &TP_PlaylistWindow::slot_moveTitleBar);
    connect(ui->frame_Title,        &TP_TitleBar::signal_leftButtonReleased,
            this,                   &TP_PlaylistWindow::slot_leftButtonReleased);
    connect(ui->playlistContainer,  &TP_PlaylistContainer::signal_resizeWindow,
            this,                   &TP_PlaylistWindow::slot_resizeWindow);
    connect(ui->playlistContainer,  &TP_PlaylistContainer::signal_leftButtonReleased,
            this,                   &TP_PlaylistWindow::slot_leftButtonReleased);
    connect(ui->frame_Bottom,       &TP_PlaylistBottomFrame::signal_resizeWindow,
            this,                   &TP_PlaylistWindow::slot_resizeWindow);
    connect(ui->frame_Bottom,       &TP_PlaylistBottomFrame::signal_leftButtonReleased,
            this,                   &TP_PlaylistWindow::slot_leftButtonReleased);
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
    if( !std::filesystem::exists( TP::configDirectoryPath.toLocal8Bit().constData() ) )
        std::filesystem::create_directory( TP::configDirectoryPath.toLocal8Bit().constData() );
}
