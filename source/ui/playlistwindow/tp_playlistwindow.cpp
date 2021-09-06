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

TP_PlaylistWindow::TP_PlaylistWindow( QWidget *parent ) :
    QWidget { parent }
  , ui { new Ui::TP_PlaylistWindow }
  , currentFileListWidget { nullptr }
  , vector_FileListWidget {}
  , previousItem { nullptr }
  , currentItem { nullptr }
  , nextItem { nullptr }
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
        emit signal_newFilelistWidgetCreated(currentFileListWidget);
    }
}

void
TP_PlaylistWindow::setBold( const QListWidgetItem &I_listWidgetItem )
{
    for( size_t i {}; i < currentFileListWidget->count(); i++ )
    {
        QUrl lURL = I_listWidgetItem.data( TP::role_URL ).value<QUrl>();
        QUrl rURL = currentFileListWidget->item( i )->data( TP::role_URL ).value<QUrl>();
        if( lURL == rURL )
        {
            QFont font = currentFileListWidget->item( i )->font();
            currentFileListWidget->item( i )->font();
            font.setBold( true );
            currentFileListWidget->item( i )->setFont( font );
            currentFileListWidget->item( i )->setBackground( QColor("#444") );
        }
        else
        {
            QFont font = currentFileListWidget->item( i )->font();
            currentFileListWidget->item( i )->font();
            font.setBold( false );
            currentFileListWidget->item( i )->setFont( font );
            currentFileListWidget->item( i )->setBackground( QColor("#777") );
        }
    }
}

void
TP_PlaylistWindow::unsetAllBolds()
{
    for ( size_t i {}; i < currentFileListWidget->count(); i++ )
    {
        QFont font = currentFileListWidget->item( i )->font();
        font.setBold( false );
        currentFileListWidget->item( i )->setFont( font );
        currentFileListWidget->item( i )->setBackground( QColor("#777") );
    }
}

void
TP_PlaylistWindow::setMode_SingleTime()
{
    playMode = TP::singleTime;
}

void
TP_PlaylistWindow::setMode_Repeat()
{
    playMode = TP::repeat;
}

void
TP_PlaylistWindow::setMode_Sequential()
{
    playMode = TP::sequential;
}

void
TP_PlaylistWindow::setMode_Shuffle()
{
    playMode = TP::shuffle;
}

QListWidgetItem *
TP_PlaylistWindow::getCurrentItem()
{
    QList <QListWidgetItem *> itemList = currentFileListWidget->selectedItems();
    if( itemList.count() == 0 )
        return getNextItem();
    else
        return itemList[0];
}

QListWidgetItem *
TP_PlaylistWindow::getNextItem()
{

}

QListWidgetItem *
TP_PlaylistWindow::getPreviousItem()
{

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
        QString qstr_extension = fileInfo.suffix().toLower();

        TagLib::FileRef fileRef { qstr_localFilePath.toLocal8Bit().constData() };
        QString qstr_title = TStringToQString( fileRef.tag()->title() );
        QString qstr_artist = TStringToQString( fileRef.tag()->artist() );
        QString qstr_album = TStringToQString( fileRef.tag()->album() );

        int duration = fileRef.audioProperties()->lengthInSeconds();

        QListWidgetItem *item = new QListWidgetItem { currentFileListWidget };

        // set URL
        item->setData( TP::role_URL, fileURL );

        // set source type
        item->setData( TP::role_SourceType, TP::single );

        // set duration
        item->setData(TP::role_Duration, duration);

        // set file name
        item->setData(TP::role_FileName, qstr_Filename);

        // set file type
        if( qstr_extension == QString("flac") )
            item->setData( TP::role_FileType, TP::FileFormat::FLAC );
        else if( qstr_extension == QString("mp3") )
            item->setData( TP::role_FileType, TP::FileFormat::MP3 );

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
    if( !std::filesystem::exists(TP::configDirectoryPath) )
        std::filesystem::create_directory(TP::configDirectoryPath);
}
