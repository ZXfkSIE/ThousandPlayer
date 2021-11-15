#include "tp_playlistwindow.h"
#include "ui_tp_playlistwindow.h"

#include "tp_globalconst.h"
#include "tp_globalvariable.h"

#include "tp_filelistwidget.h"
#include "tp_filesearchdialog.h"
#include "tp_menu.h"
#include "tp_progressdialog.h"
#include "tp_runnable_filereader.h"

#include <QDirIterator>
#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>

TP_PlaylistWindow::TP_PlaylistWindow( QWidget *parent ) :
    QWidget                 { parent }
  , ui                      { new Ui::TP_PlaylistWindow }
  , currentFileListWidget   {}
  , vec_FileListWidget      {}
  , progressDialog          { new TP_ProgressDialog {
                                tr( "Reading files..." ),   // const QString &labelText
                                tr( "Abort" ),              // const QString &cancelButtonText
                                0,                          // int minimum
                                0,                          // int maximum (will be set in the loop)
                                this } }                    // QWidget *parent = nullptr
  , b_isDescending          { false }
{
    ui->setupUi( this );
    // Qt::Tool is used for getting rid of the window tab in taskbar
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint | Qt::Tool | Qt::NoDropShadowWindowHint );



    layout_FileListFrame = new QHBoxLayout { ui->frame_FileList };
    layout_FileListFrame->setContentsMargins( 0, 0, 0, 0 );

    ui->pushButton_Close->setIcon( QIcon { ":/image/icon_Exit.svg" } );

    initializeConnection();
    initializeMenu();

    /****************************** WARNING *****************************************
     *  The method initializePlaylist()
     *  need to be executed manually after TP_MainClass initialized the connections.
     ********************************************************************************/
}

TP_PlaylistWindow::~TP_PlaylistWindow()
{
    delete ui;
}


void
TP_PlaylistWindow::initializePlaylist()
{
    /*
    if( std::filesystem::exists( TP::playlistFilePath.
#ifdef Q_OS_WIN
    toStdWString()
#else
    toLocal8Bit().constData()
#endif
    ) )
    {
        qDebug() << "Existing playlist " << TP::playlistFilePath << " found.";
    }
    else*/
    {
        qDebug() << "Existing playlist not found. Creating default playlist and filelist.";
        ui->playlistsWidget->addItem( tr( "Default" ) );
        ui->playlistsWidget->setCurrentRow( 0 );

        TP_FileListWidget *newWidget { new TP_FileListWidget{ ui->frame_FileList, tr( "Default" ) } };
        vec_FileListWidget.push_back( newWidget );
        ui->playlistsWidget->item( 0 )->setData( Qt::UserRole, QVariant::fromValue( newWidget ) );

        switchList( newWidget );
        emit signal_newFileListWidgetCreated( newWidget );
    }
}


void
TP_PlaylistWindow::setCurrentItem( QListWidgetItem * I_item )
{
    currentFileListWidget->setCurrentItem( I_item );
}


void
TP_PlaylistWindow::setCurrentItemBold()
{
    currentFileListWidget->setCurrentItemBold();
}


void
TP_PlaylistWindow::unsetCurrentItemBold()
{
    currentFileListWidget->unsetCurrentItemBold();
}



void
TP_PlaylistWindow::refreshShowingTitle( QListWidgetItem *I_item )
{
    int index {
        currentFileListWidget->indexFromItem( TP::currentItem() ).row()
    };
    currentFileListWidget->refreshShowingTitle( index, index );

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
TP_PlaylistWindow::slot_clearPreviousAndNext()
{
    currentFileListWidget->clearPreviousAndNext();
}


void
TP_PlaylistWindow::slot_activateWindow()
{
    if( isVisible() )
        raise();
}


void
TP_PlaylistWindow::slot_changeFontOfCurrentList()
{
    currentFileListWidget->setFont( TP::config().getPlaylistFont() );
}

// *****************************************************************
// private slots
// *****************************************************************

void
TP_PlaylistWindow::slot_moveTitleBar( const QRect &newGeometry )
{
    emit signal_moveWindow( this, newGeometry );
}


void
TP_PlaylistWindow::slot_leftButtonReleased()
{
    emit signal_leftButtonReleased();
}


void
TP_PlaylistWindow::slot_resizeWindow( const QRect &newGeomtry, TP::ResizeType resizeType )
{
    emit signal_resizeWindow( this, newGeomtry, resizeType );
}


void
TP_PlaylistWindow::slot_newFileListWidgetCreated( TP_FileListWidget *I_fileListWidget )
{
    emit signal_newFileListWidgetCreated( I_fileListWidget );
}


void
TP_PlaylistWindow::on_pushButton_Close_clicked()
{
    hide();
}


void TP_PlaylistWindow::on_action_addFiles_triggered()
{
    QList <QUrl> fileURLs {
        QFileDialog::getOpenFileUrls(
                    this,                                       // QWidget *parent = nullptr
                    {},                                         // const QString &caption = QString()
                    TP::config().getLastOpenedDirectory(),      // const QString &dir = QString()

                    // const QString &filter = QString()

                    tr( "All supported formats" ) + QString{ " (*.flac *alac *.m4a *.aac *.mp3 *.wav *.ogg);;" } +
                    tr( "FLAC files" ) + QString{ " (*.flac);;" } +
                    tr( "ALAC files" ) + QString{ " (*.alac);;" } +
                    tr( "AAC files" ) + QString{ " (*.m4a *.aac);;" } +
                    tr( "MP3 files" ) + QString{ " (*.mp3);;" } +
                    tr( "WAV files" ) + QString{ " (*.wav);;" } +
                    tr( "Vorbis files" ) + QString{ " (*.ogg)" }
                    )};


    if( fileURLs.isEmpty() )
        return;

    TP::config().setLastOpenedDirectory( fileURLs.front().adjusted( QUrl::RemoveFilename ) );

    addFilesToCurrentList( fileURLs );
}


void
TP_PlaylistWindow::on_action_addFolder_triggered()
{
    QUrl directoryURL {
        QFileDialog::getExistingDirectoryUrl(
                    this,                                   // QWidget *parent = nullptr
                    {},                                     // const QString &caption = QString()
                    TP::config().getLastOpenedDirectory()   // const QUrl &dir = QUrl()
                    )};

    TP::config().setLastOpenedDirectory( directoryURL );

    QList< QUrl > fileURLs {};

    QDirIterator dItr {
        directoryURL.toLocalFile(),                     // const QString &path
        { "*.flac","*.alac", "*.m4a", "*.aac",
            "*.mp3", "*.wav", "*.ogg" },                // const QStringList &nameFilters
        QDir::Files,                                    // QDir::Filters filters = QDir::NoFilter
        QDirIterator::FollowSymlinks                    // QDirIterator::IteratorFlags flags = NoIteratorFlags
    };

    while( dItr.hasNext() )
        fileURLs.push_back( QUrl::fromLocalFile( dItr.next() ) );

    addFilesToCurrentList( fileURLs );
}


void
TP_PlaylistWindow::on_action_clearSelectedItems_triggered()
{
    currentFileListWidget->slot_clearSelectedItems();
}


void
TP_PlaylistWindow::on_action_clearUnselectedItems_triggered()
{
    currentFileListWidget->clearUnselectedItems();
}


void
TP_PlaylistWindow::on_action_clearAllItems_triggered()
{
    currentFileListWidget->clearAllItems();
}


void
TP_PlaylistWindow::on_action_clearInaccessibleItems_triggered()
{
    currentFileListWidget->clearInaccessibleItems();
}


void
TP_PlaylistWindow::on_action_deleteFromDisk_triggered()
{
    currentFileListWidget->deleteSelectedItems();
}


void
TP_PlaylistWindow::on_action_selectAll_triggered()
{
    currentFileListWidget->selectAll();
}


void
TP_PlaylistWindow::on_action_unselectAll_triggered()
{
    currentFileListWidget->clearSelection();
}


void
TP_PlaylistWindow::on_action_reverseSelection_triggered()
{
    currentFileListWidget->reverseSelection();
}


void
TP_PlaylistWindow::on_action_sortByDuration_triggered()
{
    currentFileListWidget->sortByData( TP::role_Duration, b_isDescending );
}


void
TP_PlaylistWindow::on_action_sortByPath_triggered()
{
    currentFileListWidget->sortByData( TP::role_URL, b_isDescending );
}


void
TP_PlaylistWindow::on_action_sortByFilename_triggered()
{
    currentFileListWidget->sortByData( TP::role_FileName, b_isDescending );
}


void
TP_PlaylistWindow::on_action_sortByDescription_triggered()
{
    currentFileListWidget->sortByData( TP::role_Description, b_isDescending );
}


void TP_PlaylistWindow::on_action_sortByAlbum_triggered()
{
    currentFileListWidget->sortByData( TP::role_Album, b_isDescending );
}


void TP_PlaylistWindow::on_action_sortByArtist_triggered()
{
    currentFileListWidget->sortByData( TP::role_Artist, b_isDescending );
}


void TP_PlaylistWindow::on_action_sortByTitle_triggered()
{
    currentFileListWidget->sortByData( TP::role_Title, b_isDescending );
}


void
TP_PlaylistWindow::on_action_setDescending_triggered( bool checked )
{
    b_isDescending = checked;
}


void
TP_PlaylistWindow::on_action_find_triggered()
{
    if( ! currentFileListWidget->count() )
        return;

    TP_FileSearchDialog fileSearchDialog { this };
    fileSearchDialog.setModal( true );
    if( fileSearchDialog.exec() == QDialog::Accepted )
        currentFileListWidget->searchByData(
                    fileSearchDialog.getKeyword(),
                    0,
                    fileSearchDialog.isFilenameSearched(),
                    fileSearchDialog.isAlbumSearched(),
                    fileSearchDialog.isArtistSearched(),
                    fileSearchDialog.isTitleSearched()
                    );
}


void
TP_PlaylistWindow::on_action_findNext_triggered()
{
    if( ! currentFileListWidget->count() )
        return;

    currentFileListWidget->findNext();
}


// *****************************************************************
// private override
// *****************************************************************


void
TP_PlaylistWindow::showEvent( QShowEvent *event )
{
    QWidget::showEvent( event );
    emit signal_shown();
}


void
TP_PlaylistWindow::hideEvent( QHideEvent *event )
{
    QWidget::hideEvent( event );
    emit signal_hidden();
}

// *****************************************************************
// private
// *****************************************************************

void
TP_PlaylistWindow::initializeMenu()
{
    // =============== Initialize menu of "Add" button ===============
    menu_Add = new TP_Menu { ui->pushButton_Add };

    menu_Add->addAction( ui->action_addFiles );
    menu_Add->addAction( ui->action_addFolder );
    /* ----- Pending implementation ----- */
    // menu_Add->addSeparator();
    // menu_Add->addAction( ui->action_addURL );

    ui->pushButton_Add->setMenu( menu_Add );

    // =============== Initialize menu of "Remove" button ===============
    menu_Remove = new TP_Menu { ui->pushButton_Remove };

    menu_Remove->addAction( ui->action_clearSelectedItems );
    menu_Remove->addAction( ui->action_clearUnselectedItems );
    menu_Remove->addSeparator();
    menu_Remove->addAction( ui->action_clearInaccessibleItems );
    menu_Remove->addAction( ui->action_clearAllItems );
    menu_Remove->addSeparator();
    menu_Remove->addAction( ui->action_deleteFromDisk );

    ui->pushButton_Remove->setMenu( menu_Remove );

    // =============== Initialize menu of "Select" button ===============
    menu_Select = new TP_Menu { ui->pushButton_Select };

    menu_Select->addAction( ui->action_selectAll );
    menu_Select->addAction( ui->action_unselectAll );
    menu_Select->addSeparator();
    menu_Select->addAction( ui->action_reverseSelection );

    ui->pushButton_Select->setMenu( menu_Select );

    // =============== Initialize menu of "Sort" button ===============
    menu_Sort = new TP_Menu { ui->pushButton_Sort };

    menu_Sort->addAction( ui->action_sortByDuration );
    menu_Sort->addAction( ui->action_sortByPath );
    menu_Sort->addAction( ui->action_sortByFilename );
    menu_Sort->addAction( ui->action_sortByDescription );
    menu_Sort->addSeparator();
    menu_Sort->addAction( ui->action_sortByAlbum );
    menu_Sort->addAction( ui->action_sortByArtist );
    menu_Sort->addAction( ui->action_sortByTitle );
    menu_Sort->addSeparator();
    menu_Sort->addAction( ui->action_setDescending );

    ui->pushButton_Sort->setMenu( menu_Sort );

    // =============== Initialize menu of "Find" button ===============
    menu_Find = new TP_Menu { ui->pushButton_Find };

    menu_Find->addAction( ui->action_find );
    menu_Find->addAction( ui->action_findNext );

    ui->pushButton_Find->setMenu( menu_Find );
}


void
TP_PlaylistWindow::initializeConnection()
{
    // Window moving and resizing related
    connect( ui->frame_Title,       &TP_TitleBar::signal_moveTitleBar,
             this,                  &TP_PlaylistWindow::slot_moveTitleBar );
    connect( ui->frame_Title,       &TP_TitleBar::signal_leftButtonReleased,
             this,                  &TP_PlaylistWindow::slot_leftButtonReleased );
    connect( ui->playlistContainer, &TP_PlaylistContainer::signal_resizeWindow,
             this,                  &TP_PlaylistWindow::slot_resizeWindow );
    connect( ui->playlistContainer, &TP_PlaylistContainer::signal_leftButtonReleased,
             this,                  &TP_PlaylistWindow::slot_leftButtonReleased );
    connect( ui->frame_Bottom,      &TP_PlaylistBottomFrame::signal_resizeWindow,
             this,                  &TP_PlaylistWindow::slot_resizeWindow );
    connect( ui->frame_Bottom,      &TP_PlaylistBottomFrame::signal_leftButtonReleased,
             this,                  &TP_PlaylistWindow::slot_leftButtonReleased );
}

/*
void
TP_PlaylistWindow::storePlaylist()
{
    // Pending implementation
    if( ! std::filesystem::exists( TP::configDirectoryPath.
#ifdef Q_OS_WIN
    toStdWString()
#else
    toLocal8Bit().constData()
#endif
    ) )
        std::filesystem::create_directory( TP::configDirectoryPath.toStdWString() );
}
*/


void
TP_PlaylistWindow::switchList( TP_FileListWidget *fileListWidget )
{
    if( fileListWidget != currentFileListWidget )
    {
        if( currentFileListWidget )
            layout_FileListFrame->removeWidget( currentFileListWidget );
        layout_FileListFrame->addWidget( currentFileListWidget = fileListWidget );
    }

    slot_changeFontOfCurrentList();
}


void
TP_PlaylistWindow::addFilesToCurrentList( const QList< QUrl >& fileURLs )
{
    int originalCount { currentFileListWidget->count() };

    progressDialog->reset();
    progressDialog->setMaximum( fileURLs.size() );
    progressDialog->setValue( 0 );
    progressDialog->show();

    for ( const QUrl& fileURL: fileURLs )
    {
        QListWidgetItem *item = new QListWidgetItem { currentFileListWidget };
        item->setData( TP::role_URL, fileURL );         // set URL
        item->setData( TP::role_SourceType, TP::singleFile );
        currentFileListWidget->addItem( item );
    }

    const int step { 10 };
    int nextPercent { step };
    const int count { currentFileListWidget->count() };

    for ( size_t i {}; i < count; i++ )
    {
        if( i * 100 / count >= nextPercent )
        {
            nextPercent += step;
            progressDialog->setValue( i + 1 );
        }

        QThreadPool::globalInstance()->start(
                    new TP_Runnable_FileReader{ currentFileListWidget->item( i ) }
                    );

        if( progressDialog->wasCanceled() )
        {
            progressDialog->cancel();
            break;
        }
    }

    QThreadPool::globalInstance()->waitForDone();
    currentFileListWidget->refreshShowingTitle( originalCount - 1, count - 1 );
    progressDialog->cancel();
}
