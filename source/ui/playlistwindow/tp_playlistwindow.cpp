#include "tp_playlistwindow.h"
#include "ui_tp_playlistwindow.h"

#include "tp_globalconst.h"
#include "tp_globalfunction.h"
#include "tp_globalvariable.h"

#include "tp_filelistwidget.h"
#include "tp_filesearchdialog.h"
#include "tp_menu.h"
#include "tp_progressdialog.h"
#include "tp_replaygainscanprogress.h"
#include "tp_runnable_filereader.h"

#include <QDirIterator>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QMouseEvent>
#include <QThreadPool>

TP_PlaylistWindow::TP_PlaylistWindow( QWidget *parent ) :
    QWidget                 { parent }
  , ui                      { new Ui::TP_PlaylistWindow }
  , progressDialog          { new TP_ProgressDialog {
                                tr( "Reading files..." ),   // const QString &labelText
                                tr( "Abort" ),              // const QString &cancelButtonText
                                0,                          // int minimum
                                0,                          // int maximum (will be set in the loop)
                                this } }                    // QWidget *parent = nullptr
  , replayGainScanProgress  { new TP_ReplayGainScanProgress }
  , b_isDescending          { false }
{
    ui->setupUi( this );
    // Qt::Tool is used for getting rid of the window tab in taskbar
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint | Qt::Tool | Qt::NoDropShadowWindowHint );

    ui->pushButton_Close->setIcon( QIcon { ":/image/icon_Exit.svg" } );

    initializeMenu();
    initializeConnection();
    initializePlaylist();

    slot_refreshFont();
}


TP_PlaylistWindow::~TP_PlaylistWindow()
{
    storePlaylist();

    delete ui;
}


void
TP_PlaylistWindow::setCurrentItem( QListWidgetItem * I_item )
{
    currentFileListWidget()->setCurrentItem( I_item );
}


void
TP_PlaylistWindow::setCurrentItemBold()
{
    currentFileListWidget()->setCurrentItemBold();
}


void
TP_PlaylistWindow::unsetCurrentItemBold()
{
    currentFileListWidget()->unsetCurrentItemBold();
}



void
TP_PlaylistWindow::refreshItemShowingTitle( QListWidgetItem *I_item )
{
    auto index {
        currentFileListWidget()->indexFromItem( TP::currentItem() ).row()
    };
    currentFileListWidget()->refreshShowingTitle( index, index );

}


QListWidgetItem *
TP_PlaylistWindow::getCurrentItem()
{
    return currentFileListWidget()->getCurrentItem();
}


QListWidgetItem *
TP_PlaylistWindow::getNextItem()
{
    return currentFileListWidget()->getNextItem();
}


QListWidgetItem *
TP_PlaylistWindow::getPreviousItem()
{
    return currentFileListWidget()->getPreviousItem();
}


QListWidgetItem *
TP_PlaylistWindow::getNextItem_shuffle()
{
    return currentFileListWidget()->getNextItem_shuffle();
}


QListWidgetItem *
TP_PlaylistWindow::getPreviousItem_shuffle()
{
    return currentFileListWidget()->getPreviousItem_shuffle();
}

// *****************************************************************
// public slots
// *****************************************************************

void
TP_PlaylistWindow::slot_clearPreviousAndNext()
{
    currentFileListWidget()->clearPreviousAndNext();
}


void
TP_PlaylistWindow::slot_activateWindow()
{
    if( isVisible() )
        raise();
}


void
TP_PlaylistWindow::slot_refreshFont()
{
    for( unsigned i {}; i < ui->stackedWidget_FileList->count(); i++ )
    {
        dynamic_cast< TP_FileListWidget * >( ui->stackedWidget_FileList->widget( i ) )->refreshFont();
    }

    ui->playlistsWidget->refreshFont();
}

// *****************************************************************
// private slots
// *****************************************************************

void
TP_PlaylistWindow::slot_titleBarMoved( const QRect &I_geometry )
{
    emit signal_moveWindow( this, I_geometry );
}


void
TP_PlaylistWindow::slot_resizeWindow( const QRect &I_geometry, TP::CursorPositionType I_resizeType )
{
    emit signal_resizeWindow( this, I_geometry, I_resizeType );
}


void
TP_PlaylistWindow::slot_windowChanged()
{
    emit signal_windowChanged();
}


void
TP_PlaylistWindow::slot_fileListRemoved( TP_FileListWidget *I_fileListWidget )
{
    ui->stackedWidget_FileList->removeWidget( I_fileListWidget );
    delete I_fileListWidget;
}


void
TP_PlaylistWindow::slot_fileListCreated( TP_FileListWidget *I_fileListWidget )
{
    connect( I_fileListWidget,  &TP_FileListWidget::signal_currentItemRemoved,
             this,              &TP_PlaylistWindow::slot_currentItemRemoved );
    connect( I_fileListWidget,  &TP_FileListWidget::itemDoubleClicked,
             this,              &TP_PlaylistWindow::slot_itemDoubleClicked );

    I_fileListWidget->setParent( ui->stackedWidget_FileList );
    ui->stackedWidget_FileList->addWidget( I_fileListWidget );
}


void
TP_PlaylistWindow::slot_fileListSwitched( TP_FileListWidget *I_fileListWidget )
{
    currentFileListWidget()->unsetCurrentItemBold();
    ui->stackedWidget_FileList->setCurrentWidget( I_fileListWidget );
    TP::currentItem() = nullptr;
    emit signal_currentItemRemoved();
}


void
TP_PlaylistWindow::slot_currentItemRemoved()
{
    emit signal_currentItemRemoved();
}


void
TP_PlaylistWindow::slot_itemDoubleClicked( QListWidgetItem *I_item )
{
    emit signal_itemDoubleClicked( I_item );
}


void
TP_PlaylistWindow::on_pushButton_Close_clicked()
{
    hide();
}


void TP_PlaylistWindow::on_action_addFiles_triggered()
{
    const auto &fileURLs {
        QFileDialog::getOpenFileUrls(
                    this,                                       // QWidget *parent = nullptr
                    {},                                         // const QString &caption = QString()
                    TP::config().getLastOpenedDirectory(),      // const QString &dir = QString()

                    // const QString &filter = QString()

                    tr( "All supported formats" )
#ifdef Q_OS_LINUX
                    + QString{ " (*.flac *alac *.m4a *.aac *.mp3 *.wav *.ogg);;" }
#else
                    // Currently, QtMultimedia does not support Ogg Vorbis files under Windows.
                    // See https://bugreports.qt.io/browse/QTBUG-99278

                    + QString{ " (*.flac *alac *.m4a *.aac *.mp3 *.wav);;" }
#endif
                    + tr( "FLAC files" ) + QString{ " (*.flac);;" }
                    + tr( "ALAC files" ) + QString{ " (*.alac);;" }
                    + tr( "AAC files" ) + QString{ " (*.m4a *.aac);;" }
                    + tr( "MP3 files" ) + QString{ " (*.mp3);;" }
                    + tr( "WAV files" ) + QString{ " (*.wav);;" }
#ifdef Q_OS_LINUX
                    + tr( "Vorbis files" ) + QString{ " (*.ogg)" }
#endif
                    )};


    if( fileURLs.isEmpty() )
        return;

    TP::config().setLastOpenedDirectory( fileURLs.front().adjusted( QUrl::RemoveFilename ) );

    addFilesToCurrentList( fileURLs );
}


void
TP_PlaylistWindow::on_action_addFolder_triggered()
{
    const auto &directoryURL {
        QFileDialog::getExistingDirectoryUrl(
                    this,                                   // QWidget *parent = nullptr
                    {},                                     // const QString &caption = QString()
                    TP::config().getLastOpenedDirectory()   // const QUrl &dir = QUrl()
                    )};

    if( directoryURL.isEmpty() )
        return;

    TP::config().setLastOpenedDirectory( directoryURL );

    QList< QUrl > fileURLs {};

    QDirIterator dItr {
        directoryURL.toLocalFile(),                     // const QString &path
        // const QStringList &nameFilters
#ifdef Q_OS_LINUX
        { "*.flac","*.alac", "*.m4a", "*.aac", "*.mp3", "*.wav", "*.ogg" },
#else
        { "*.flac","*.alac", "*.m4a", "*.aac", "*.mp3", "*.wav" },
#endif
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
    currentFileListWidget()->slot_clearSelectedItems();
}


void
TP_PlaylistWindow::on_action_clearUnselectedItems_triggered()
{
    currentFileListWidget()->clearUnselectedItems();
}


void
TP_PlaylistWindow::on_action_clearAllItems_triggered()
{
    currentFileListWidget()->clearAllItems();
}


void
TP_PlaylistWindow::on_action_clearInaccessibleItems_triggered()
{
    currentFileListWidget()->clearInaccessibleItems();
}


void
TP_PlaylistWindow::on_action_deleteFromDisk_triggered()
{
    currentFileListWidget()->deleteSelectedItems();
}


void
TP_PlaylistWindow::on_action_selectAll_triggered()
{
    currentFileListWidget()->selectAll();
}


void
TP_PlaylistWindow::on_action_unselectAll_triggered()
{
    currentFileListWidget()->clearSelection();
}


void
TP_PlaylistWindow::on_action_reverseSelection_triggered()
{
    currentFileListWidget()->reverseSelection();
}


void
TP_PlaylistWindow::on_action_sortByDuration_triggered()
{
    currentFileListWidget()->sortByData( TP::role_Duration, b_isDescending );
}


void
TP_PlaylistWindow::on_action_sortByPath_triggered()
{
    currentFileListWidget()->sortByData( TP::role_URL, b_isDescending );
}


void
TP_PlaylistWindow::on_action_sortByFilename_triggered()
{
    currentFileListWidget()->sortByData( TP::role_FileName, b_isDescending );
}


void
TP_PlaylistWindow::on_action_sortByDescription_triggered()
{
    currentFileListWidget()->sortByData( TP::role_Description, b_isDescending );
}

void
TP_PlaylistWindow::on_action_sortByLastModified_triggered()
{
    currentFileListWidget()->sortByData( TP::role_LastModified, b_isDescending );
}

void TP_PlaylistWindow::on_action_sortByAlbum_triggered()
{
    currentFileListWidget()->sortByData( TP::role_Album, b_isDescending );
}


void TP_PlaylistWindow::on_action_sortByArtist_triggered()
{
    currentFileListWidget()->sortByData( TP::role_Artist, b_isDescending );
}


void TP_PlaylistWindow::on_action_sortByTitle_triggered()
{
    currentFileListWidget()->sortByData( TP::role_Title, b_isDescending );
}


void
TP_PlaylistWindow::on_action_setDescending_triggered( bool I_isChecked )
{
    b_isDescending = I_isChecked;
}


void
TP_PlaylistWindow::on_action_find_triggered()
{
    if( ! currentFileListWidget()->count() )
        return;

    TP_FileSearchDialog fileSearchDialog { this };
    fileSearchDialog.setModal( true );
    if( fileSearchDialog.exec() == QDialog::Accepted )
        currentFileListWidget()->searchByData(
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
    if( ! currentFileListWidget()->count() )
        return;

    currentFileListWidget()->findNext();
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
    menu_Sort->addAction( ui->action_sortByLastModified );
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
             this,                  &TP_PlaylistWindow::slot_titleBarMoved );
    connect( ui->frame_Title,       &TP_TitleBar::signal_leftButtonReleased,
             this,                  &TP_PlaylistWindow::slot_windowChanged );
    connect( ui->playlistContainer, &TP_PlaylistContainer::signal_resizeWindow,
             this,                  &TP_PlaylistWindow::slot_resizeWindow );
    connect( ui->playlistContainer, &TP_PlaylistContainer::signal_windowChanged,
             this,                  &TP_PlaylistWindow::slot_windowChanged );
    connect( ui->frame_Bottom,      &TP_PlaylistBottomFrame::signal_resizeWindow,
             this,                  &TP_PlaylistWindow::slot_resizeWindow );
    connect( ui->frame_Bottom,      &TP_PlaylistBottomFrame::signal_windowChanged,
             this,                  &TP_PlaylistWindow::slot_windowChanged );

    // PlaylistsWidget related
    connect( ui->playlistsWidget,   &TP_PlaylistsWidget::signal_fileListCreated,
             this,                  &TP_PlaylistWindow::slot_fileListCreated );
    connect( ui->playlistsWidget,   &TP_PlaylistsWidget::signal_fileListRemoved,
             this,                  &TP_PlaylistWindow::slot_fileListRemoved );
    connect( ui->playlistsWidget,   &TP_PlaylistsWidget::signal_fileListSwitched,
             this,                  &TP_PlaylistWindow::slot_fileListSwitched );
}


bool
TP_PlaylistWindow::createPlaylistFromJSON( const QJsonDocument &I_jDoc )
{
    // The braces {} should not be used here
    // since they will trigger std::initializer_list constructor.
    const auto &jArray_Root( I_jDoc.array() );

    bool isPlaylistCreated { false };

    for( const auto &jValue_Playlist : jArray_Root )
    {
        const auto &jObject_Playlist { jValue_Playlist.toObject() };
        if( jObject_Playlist.isEmpty() )
            continue;

        auto listName { jObject_Playlist[ key_listName ].toString() };
        if( listName.isEmpty() )
            listName = tr( "List " ) + QString::number( ui->playlistsWidget->count() + 1 );

        auto *newPlaylist { ui->playlistsWidget->addNewList( listName ) };
        isPlaylistCreated = true;

        const auto &jValue_FileList { jObject_Playlist[ key_fileList ] };
        auto jArray_FileList ( jValue_FileList.toArray() );

        if( jArray_FileList.isEmpty() )
            continue;

        auto *newFileList {
            reinterpret_cast< TP_FileListWidget * >(
                        newPlaylist->data( TP::role_FileListAddress ).value< quintptr >()
                        )
        };

        for( int i {}; i < jArray_FileList.count(); i++ )
        {
            const auto &jObject_File { jArray_FileList.at( i ).toObject() };
            TP::SourceType sourceType {
                static_cast< TP::SourceType >( jObject_File[ key_sourceType ].toInt() )
            };
            QUrl fileURL {  jObject_File[ key_url ].toString() };
            auto AudioFormat { TP::getAudioFormat( fileURL ) };

            if( ! fileURL.isEmpty() && AudioFormat != TP::AudioFormat::NotSupported )
            {
                auto *item = new QListWidgetItem { newFileList };
                item->setData( TP::role_URL, fileURL );             // set URL
                item->setData( TP::role_AudioFormat, static_cast< int >( AudioFormat ) );
                item->setData( TP::role_SourceType, static_cast< int >( sourceType ) );
                newFileList->addItem( item );
            }
        }       // for( int i {}; i < jArray_FileList.count(); i++ )

        auto count { newFileList->count() };
        if( ! count )
            continue;

        int nextPercent { percentageStep };

        progressDialog->reset();
        progressDialog->setMaximum( ! newFileList->count() );
        progressDialog->setValue( 0 );
        progressDialog->show();

        for ( unsigned i {}; i < count; i++ )
            QThreadPool::globalInstance()->start(
                        new TP_Runnable_FileReader { newFileList->item( i ) }
                        );

        while( auto n_activeThread = QThreadPool::globalInstance()->activeThreadCount() )
        {
            auto countFinished { count - n_activeThread };
            auto currentPercentage { countFinished * 100 / count };
            if( currentPercentage >= nextPercent )
            {
                while( currentPercentage >= nextPercent )
                    nextPercent += percentageStep;
                progressDialog->setValue( countFinished );
            }
        }

        QThreadPool::globalInstance()->waitForDone();
        progressDialog->cancel();

        newFileList->refreshShowingTitle( 0, count - 1 );
    }       // for( const auto &jValue_Playlist : jArray_Root )

    return isPlaylistCreated;
}


void
TP_PlaylistWindow::initializePlaylist()
{
    if( std::filesystem::exists( TP::playlistFilePath.
#ifdef Q_OS_WIN
    toStdWString()
#else
    toLocal8Bit().constData()
#endif
    ) )
    {
        QFile qFile { TP::playlistFilePath };
        qDebug() << "[PlaylistWindow] Reading playlists from" << qFile.fileName();
        if( ! qFile.open( QFile::ReadOnly ) )
        {
            QMessageBox::critical(
                        this,
                        tr( "Playlist Reading Error" ),
                        tr( "Failed to read playlists from %1.\n" ).arg( TP::playlistFilePath )
                        + qFile.errorString() + "\n"
                        + tr( "Default playlist will be created instead." )
                        );
            goto CREATE_DEFAULT_LISTS;
        }

        QJsonParseError jsonParseError {};
        const auto &jDoc {
            QJsonDocument::fromJson( qFile.readAll(), &jsonParseError ),
        };

        if( jDoc.isNull() )
        {
            QMessageBox::critical(
                        this,
                        tr( "Playlist Reading Error" ),
                        tr( "Failed to read playlists from %1.\n" ).arg( TP::playlistFilePath )
                        + tr( "Empty QJsonDocument object returned.\n" )
                        + tr( "Default playlist will be created instead." )
                        );
            goto CREATE_DEFAULT_LISTS;
        }
        else if( jsonParseError.error != QJsonParseError::NoError )
        {
            QMessageBox::critical(
                        this,
                        tr( "Playlist Reading Error" ),
                        tr( "Failed to read playlists from %1.\n" ).arg( TP::playlistFilePath )
                        + jsonParseError.errorString()
                        + tr( " (offset: %1)\n" ).arg( jsonParseError.offset )
                        + tr( "Default playlist will be created instead." )
                        );
            goto CREATE_DEFAULT_LISTS;
        }

        if( ! createPlaylistFromJSON( jDoc ) )
        {
            QMessageBox::critical(
                        this,
                        tr( "Playlist Reading Error" ),
                        tr( "Failed to read playlists from %1.\n" ).arg( TP::playlistFilePath )
                        + tr( "No valid playlist found.\n" )
                        + tr( "Default playlist will be created instead." )
                        );
            goto CREATE_DEFAULT_LISTS;
        }
    }
    else
    {
CREATE_DEFAULT_LISTS:
        ui->playlistsWidget->addNewList( tr( "Default" ) );
    }
}


void
TP_PlaylistWindow::storePlaylist()
{
    QJsonArray jArray_Root {};
    auto playlistsCount { ui->playlistsWidget->count() };

    for( unsigned i {}; i < playlistsCount; i++ )
    {
        auto *currentPlaylistItem { ui->playlistsWidget->item( i ) };
        QJsonObject jObject_Playlist {};
        jObject_Playlist[ key_listName ] = currentPlaylistItem->text();

        QJsonArray jArray_FileList {};
        auto *currentFileList {
            reinterpret_cast< TP_FileListWidget * >(
                        currentPlaylistItem->data( TP::role_FileListAddress ).value< quintptr >()
                        )
        };
        auto filesCount { currentFileList->count() };

        for( unsigned j {}; j < filesCount; j++ )
        {
            QJsonObject jObject_File {};
            auto *currentFile { currentFileList->item( j ) };

            jObject_File[ key_sourceType ]  = currentFile->data( TP::role_SourceType ).toInt();
            jObject_File[ key_url ]         = currentFile->data( TP::role_URL ).toUrl().toString();

            jArray_FileList.append( jObject_File );
        }

        jObject_Playlist[ key_fileList ] = jArray_FileList;
        jArray_Root.append( jObject_Playlist );
    }

    // Save it to File
    QJsonDocument jDoc { jArray_Root };

    QFile jsonFile { TP::playlistFilePath };
    qDebug() << "[PlaylistWindow] Saving playlists to" << TP::playlistFilePath;

    if( jsonFile.open( QIODevice::WriteOnly )
            && jsonFile.write( jDoc.toJson( QJsonDocument::Compact ) ) != -1 )
    {}
    else
        QMessageBox::critical(
                    this,
                    tr( "Playlist Writing Error" ),
                    tr( "Failed to write playlists to %1.\n" ).arg( TP::playlistFilePath )
                    + jsonFile.errorString()
                    );

}


TP_FileListWidget *
TP_PlaylistWindow::currentFileListWidget()
{
    return static_cast< TP_FileListWidget * >( ui->stackedWidget_FileList->currentWidget() );
}


void
TP_PlaylistWindow::addFilesToCurrentList( const QList< QUrl > &I_urlList )
{
    auto originalCount { currentFileListWidget()->count() };

    for ( const auto &fileURL: I_urlList )
    {
        auto AudioFormat { TP::getAudioFormat( fileURL ) };
        if( AudioFormat == TP::AudioFormat::NotSupported )
            continue;

        auto *item = new QListWidgetItem { currentFileListWidget() };
        item->setData( TP::role_URL, fileURL );
        item->setData( TP::role_AudioFormat, static_cast< int >( AudioFormat ) );
        item->setData( TP::role_SourceType, static_cast< int >( TP::SourceType::SingleFile ) );
        currentFileListWidget()->addItem( item );
    }

    auto newCount { currentFileListWidget()->count() };
    auto increasedCount { newCount - originalCount };
    if( ! increasedCount )
        return;

    int nextPercentage { percentageStep };

    // Indexes of the new added items are between [originalCount, newCount - 1]

    progressDialog->reset();
    progressDialog->setMaximum( I_urlList.size() );
    progressDialog->setValue( 0 );
    progressDialog->show();

    for( unsigned i {}; i < increasedCount; i++ )
        QThreadPool::globalInstance()->start(
                    new TP_Runnable_FileReader{ currentFileListWidget()->item( originalCount + i ) }
                    );

    while( auto n_activeThread = QThreadPool::globalInstance()->activeThreadCount() )
    {
        auto countFinished { increasedCount - n_activeThread };
        auto currentPercentage { countFinished * 100 / increasedCount };
        if( currentPercentage >= nextPercentage )
        {
            while( currentPercentage >= nextPercentage )
                nextPercentage += percentageStep;
            progressDialog->setValue( countFinished );
        }
    }

    QThreadPool::globalInstance()->waitForDone();
    progressDialog->cancel();

    currentFileListWidget()->refreshShowingTitle( originalCount, newCount - 1 );
}

