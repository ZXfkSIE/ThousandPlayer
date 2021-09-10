#include "tp_mainclass.h"

#include "tp_globalconst.h"
#include "tp_globalenum.h"
#include "tp_globalfunction.h"
#include "tp_globalvariable.h"

#include "tp_mainwindow.h"

#include "tp_playlistwindow.h"
#include "tp_filelistwidget.h"

#include <QAbstractButton>
#include <QApplication>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QListWidgetItem>
#include <QMediaDevices>
#include <QMediaMetaData>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>
#include <QWindow>

#include <cmath>
#include <deque>
#include <filesystem>

// Headers of TagLib
#include <attachedpictureframe.h>
#include <id3v2frame.h>
#include <id3v2tag.h>
#include <flacfile.h>
#include <mpegfile.h>

TP_MainClass::TP_MainClass() :
    QObject { nullptr }
  , mainWindow { new TP_MainWindow {} }
  , playlistWindow { new TP_PlaylistWindow {} }
  , b_isPlaylistWindowVisible { true }
  , audioOutput { new QAudioOutput { this } }
  , mediaPlayer { new QMediaPlayer { this } }
  , snapStatus { false }
  , snapPosition_playlistWindow {}
{
    mediaPlayer->setAudioOutput( audioOutput );
    qDebug()<< "Current audio output device is "<< mediaPlayer->audioOutput()->device().description();

    initializeConnection();

    mainWindow->show();
    mainWindow->setVolumeSliderValue( 50 );

    playlistWindow->initializePlaylist();       // Must be executed before showing
    playlistWindow->show();
}


TP_MainClass::~TP_MainClass()
{
    QScreen *mainWindowScreen = QApplication::screenAt( mainWindow->pos() );
    TP::Config().setMainWindowPosition( mainWindow->pos() - mainWindowScreen->geometry().topLeft() );

    QScreen *playlistWindowScreen = QApplication::screenAt( playlistWindow->pos() );
    TP::Config().setPlaylistWindowPosition( playlistWindow->pos() - playlistWindowScreen->geometry().topLeft() );

    delete mainWindow;
    delete playlistWindow;
}

// *****************************************************************
// public slots:
// *****************************************************************


// Need to be executed manually after QApplication object executed its exec()
void
TP_MainClass::slot_checkIfServiceAvailable()
{
    if ( ! mediaPlayer->isAvailable() )
    {
        QMessageBox msgBox_ServiceNotAvailable (
                    QMessageBox::Critical,
                    tr("Service not available"),
                    tr("The QMediaPlayer object does not have a valid service.\n"
                       "Please check the media service plugins are installed."),
                    QMessageBox::NoButton );
        msgBox_ServiceNotAvailable.addButton(
                    tr("Exit"),
                    QMessageBox::AcceptRole );
        connect( &msgBox_ServiceNotAvailable, &QMessageBox::buttonClicked, qApp, &QApplication::quit );

        msgBox_ServiceNotAvailable.exec();
    }
}


void
TP_MainClass::slot_initializePosition()
{
    QScreen *currentScreen = QApplication::screenAt( QCursor::pos() );
    mainWindow->move( TP::Config().getMainWindowPosition() + currentScreen->geometry().topLeft() );
    playlistWindow->move( TP::Config().getPlaylistWindowPosition() + currentScreen->geometry().topLeft() );
    slot_leftButtonReleased();
}


// *****************************************************************
// private slots:
// *****************************************************************


void
TP_MainClass::slot_minimizeWindow()
{
    if( playlistWindow->isVisible() )
    {
        b_isPlaylistWindowVisible = true;
        playlistWindow->hide();
    }
    else
        b_isPlaylistWindowVisible = false;
}

void
TP_MainClass::slot_restoreWindow()
{
    if( b_isPlaylistWindowVisible )
    {
        playlistWindow->show();
        playlistWindow->raise();
    }

    slot_leftButtonReleased();
}


void
TP_MainClass::slot_itemDoubleClicked( QListWidgetItem *I_item )
{
    // Double clicking list item interrupts the playing order.
    // Therefore, previous and next items need to be initialized.
    playlistWindow->slot_modeIsNotShuffle();

    playItem( I_item );
}


void
TP_MainClass::slot_moveWindow( QWidget *window, QRect newGeometry )
{
    // A window can only be snapped (or aligned) once at vertical and horizontal direction, respectively.
    bool isVerticallySnapped = false;
    bool isHorizontallySnapped = false;

    // ============================== Snapping to main window ==============================

    if ( window != mainWindow )
    {
        TP::SnapType snapType { checkSnapType( newGeometry, mainWindow->geometry() ) };

        // --------------------- Step 1 (edge sticks to edge) ---------------------

        switch( snapType )
        {
        case TP::toBottom :
            newGeometry.moveTop( mainWindow->geometry().bottom() + 1 );
            isVerticallySnapped = true;
            break;

        case TP::toTop :
            newGeometry.moveBottom( mainWindow->geometry().top() - 1 );
            isVerticallySnapped = true;
            break;

        case TP::toRight :
            newGeometry.moveLeft( mainWindow->geometry().right() + 1 );
            isHorizontallySnapped = true;
            break;

        case TP::toLeft :
            newGeometry.moveRight( mainWindow->geometry().left() - 1 );
            isHorizontallySnapped = true;
            break;

        case TP::notAdjacent :
            goto MOVE_End_of_snap_to_main_window;
        }

        // ---------- Step 2 (edge alignment in the vertical direction) ----------

        switch( snapType )
        {
        case TP::toTop :
        case TP::toBottom :

            // Left edge alignment
            if( std::abs( newGeometry.left() - mainWindow->geometry().left() )
                    < TP::snapRange )
            {
                newGeometry.moveLeft( mainWindow->geometry().left() );
                isHorizontallySnapped = true;
                break;
            }

            // Right edge alignment
            if( std::abs( newGeometry.right() - mainWindow->geometry().right() )
                    < TP::snapRange )
            {
                newGeometry.moveRight( mainWindow->geometry().right() );
                isHorizontallySnapped = true;
                break;
            }

            break;

        case TP::toLeft :
        case TP::toRight :

            // Top edge alignment
            if( std::abs( newGeometry.top() - mainWindow->geometry().top() )
                    < TP::snapRange )
            {
                newGeometry.moveTop( mainWindow->geometry().top() );
                isVerticallySnapped = true;
                break;
            }

            // Bottom edge alignment
            if( std::abs( newGeometry.bottom() - mainWindow->geometry().bottom() )
                    < TP::snapRange )
            {
                newGeometry.moveBottom( mainWindow->geometry().bottom() );
                isVerticallySnapped = true;
                break;
            }

            break;

        default:
            break;
        }       // switch( snapType )
    }       // if( window != mainWindow )

MOVE_End_of_snap_to_main_window:

    if( isVerticallySnapped && isHorizontallySnapped )
        goto Move_window;

    // ============================== Snapping to playlist window ==============================

    if( window != playlistWindow && playlistWindow->isVisible() )
    {
        // If it is main window and playlist window has already been snapped to it, jump.
        if( window == mainWindow && breadthFirstSearch( TP::playlistWindow ) )
            goto MOVE_End_of_snap_to_playlist_window;

        // ------------------ Step 1 (edge sticks to edge) ------------------

        TP::SnapType snapType { checkSnapType( newGeometry, playlistWindow->geometry() ) };

        switch( snapType )
        {
        case TP::toBottom :
            newGeometry.moveTop( playlistWindow->geometry().bottom() + 1 );
            isVerticallySnapped = true;
            break;

        case TP::toTop :
            newGeometry.moveBottom( playlistWindow->geometry().top() - 1 );
            isVerticallySnapped = true;
            break;

        case TP::toRight :
            newGeometry.moveLeft( playlistWindow->geometry().right() + 1 );
            isHorizontallySnapped = true;
            break;

        case TP::toLeft :
            newGeometry.moveRight( playlistWindow->geometry().left() - 1 );
            isHorizontallySnapped = true;
            break;

        case TP::notAdjacent :
            goto MOVE_End_of_snap_to_playlist_window;
        }

        // ---------- Step 2 (edge alignment in the vertical direction) ----------

        switch( snapType )
        {
        case TP::toTop :
        case TP::toBottom :

            // Left edge alignment
            if( std::abs( newGeometry.left() - playlistWindow->geometry().left() )
                    <= TP::snapRange
                && !isHorizontallySnapped )
            {
                newGeometry.moveLeft( playlistWindow->geometry().left() );
                isHorizontallySnapped = true;
                break;
            }

            // Right edge alignment
            if( std::abs( newGeometry.right() - playlistWindow->geometry().right() )
                    <= TP::snapRange
                && !isHorizontallySnapped )
            {
                newGeometry.moveRight( playlistWindow->geometry().right() );
                isHorizontallySnapped = true;
                break;
            }

            break;

        case TP::toLeft :
        case TP::toRight :

            // Top edge alignment
            if( std::abs( newGeometry.top() - playlistWindow->geometry().top() )
                    <= TP::snapRange
                && !isVerticallySnapped )
            {
                newGeometry.moveTop( playlistWindow->geometry().top() );
                isVerticallySnapped = true;
                break;
            }

            // Bottom edge alignment
            if( std::abs( newGeometry.bottom() - playlistWindow->geometry().bottom() )
                    <= TP::snapRange
                && !isVerticallySnapped )
            {
                newGeometry.moveBottom( playlistWindow->geometry().bottom() );
                isVerticallySnapped = true;
                break;
            }

            break;

        default:
            break;
        }       // switch( snapType )
    }       // if( window != playlistWindow && playlistWindow->isVisible() )

MOVE_End_of_snap_to_playlist_window:

    if( isVerticallySnapped && isHorizontallySnapped )
        goto Move_window;

// ==================== Move window ====================
Move_window:

    window->setGeometry( newGeometry );
    if( window == mainWindow )
        // If window is main window, move the snapped window(s) as well
    {
        unsnapInvisibleWindows();

        // Move playlist window
        if ( breadthFirstSearch( TP::playlistWindow ) )
            playlistWindow->move( newGeometry.topLeft() + snapPosition_playlistWindow );

        // Move other windows...
    }
}


void
TP_MainClass::slot_resizeWindow( QWidget *window, QRect newGeometry, TP::ResizeType resizeType )
{
    // While resizing, a window can only be snapped (or aligned) once.
    bool            isSnapped { false };
    TP::SnapType    snapType;

    if ( window != mainWindow )
    {
        TP::SnapType snapType { checkSnapType( newGeometry, mainWindow->geometry() ) };
        TP::SnapType adjacentType { checkAdjacentType( newGeometry, mainWindow->geometry() ) };

        if( snapType == TP::notAdjacent && adjacentType == TP::notAdjacent )
            goto RESIZE_End_of_snap_to_main_window;

        if( snapType != TP::notAdjacent )
            switch( snapType )
            {
            case TP::toTop :
                if( resizeType == TP::atBottom )
                {
                    newGeometry.setBottom( mainWindow->geometry().top() - 1 );
                    isSnapped = true;
                    goto RESIZE_End_of_snap_to_main_window;
                }
                break;

            case TP::toRight :
                if( resizeType == TP::atLeft )
                {
                    newGeometry.setLeft( mainWindow->geometry().right() + 1 );
                    isSnapped = true;
                    goto RESIZE_End_of_snap_to_main_window;
                }
                break;

            case TP::toLeft :
                if( resizeType == TP::atRight )
                {
                    newGeometry.setRight( mainWindow->geometry().left() - 1 );
                    isSnapped = true;
                    goto RESIZE_End_of_snap_to_main_window;
                }
                break;

            default :
                break;
            }       // switch( snapType )

            // ---------- Step 2 (edge alignment in the vertical direction) ----------
        switch( adjacentType )
        {
        case TP::toTop :
        case TP::toBottom :

            // Left edge alignment
            if( std::abs( newGeometry.left() - mainWindow->geometry().left() )
                    <= TP::snapRange
                    && resizeType == TP::atLeft )
            {
                newGeometry.setLeft( mainWindow->geometry().left() );
                isSnapped = true;
                break;
            }

            // Right edge alignment
            if( std::abs( newGeometry.right() - mainWindow->geometry().right() )
                    <= TP::snapRange
                && resizeType == TP::atRight )
            {
                newGeometry.setRight( mainWindow->geometry().right() );
                isSnapped = true;
                break;
            }

            break;

        case TP::toLeft :
        case TP::toRight :

            // Bottom edge alignment
            if( std::abs( newGeometry.bottom() - mainWindow->geometry().bottom() )
                    <= TP::snapRange
                && resizeType == TP::atBottom )
            {
                newGeometry.setBottom( mainWindow->geometry().bottom() );
                isSnapped = true;
                break;
            }

            break;

        default:
            break;
        }       // switch( snapType )
    }   //if ( window != mainWindow )

RESIZE_End_of_snap_to_main_window:

    if( isSnapped )
        goto Resize_window;

    // ============================== Snapping to playlist window ==============================

    if( window != playlistWindow && playlistWindow->isVisible() )
    {
        // ------------------ Step 1 (edge sticks to edge) ------------------

        TP::SnapType snapType { checkSnapType( newGeometry, playlistWindow->geometry() ) };
        TP::SnapType adjacentType { checkAdjacentType( newGeometry, playlistWindow->geometry() ) };

        if( snapType == TP::notAdjacent && adjacentType == TP::notAdjacent )
            goto RESIZE_End_of_main_window_snaps_to_playlist_window;

        if( snapType != TP::notAdjacent )
            switch( snapType )
            {
            case TP::toTop :
                if( resizeType == TP::atBottom )
                {
                    newGeometry.setBottom( playlistWindow->geometry().top() - 1 );
                    isSnapped = true;
                    goto RESIZE_End_of_main_window_snaps_to_playlist_window;
                }
                break;

            case TP::toRight :
                if( resizeType == TP::atLeft )
                {
                    newGeometry.setLeft( playlistWindow->geometry().right() + 1 );
                    isSnapped = true;
                    goto RESIZE_End_of_main_window_snaps_to_playlist_window;
                }
                break;

            case TP::toLeft :
                if( resizeType == TP::atRight )
                {
                    newGeometry.setRight( playlistWindow->geometry().left() - 1 );
                    isSnapped = true;
                    goto RESIZE_End_of_main_window_snaps_to_playlist_window;
                }
                break;

            default :
                break;
            }       // switch( snapType )

            // ---------- Step 2 (edge alignment in the vertical direction) ----------
        switch( adjacentType )
        {
        case TP::toTop :
        case TP::toBottom :

            // Left edge alignment
            if( std::abs( newGeometry.left() - playlistWindow->geometry().left() )
                    <= TP::snapRange
                    && resizeType == TP::atLeft )
            {
                newGeometry.setLeft( playlistWindow->geometry().left() );
                isSnapped = true;
                break;
            }

            // Right edge alignment
            if( std::abs( newGeometry.right() - playlistWindow->geometry().right() )
                    <= TP::snapRange
                && resizeType == TP::atRight )
            {
                newGeometry.setRight( playlistWindow->geometry().right() );
                isSnapped = true;
                break;
            }

            break;

        case TP::toLeft :
        case TP::toRight :

            // Bottom edge alignment
            if( std::abs( newGeometry.bottom() - playlistWindow->geometry().bottom() )
                    <= TP::snapRange
                && resizeType == TP::atBottom )
            {
                newGeometry.setBottom( playlistWindow->geometry().bottom() );
                isSnapped = true;
                break;
            }

            break;

        default:
            break;
        }       // switch( snapType )
    }       // if( window != playlistWindow && playlistWindow->isVisible() )

RESIZE_End_of_main_window_snaps_to_playlist_window:

    if( isSnapped )
        goto Resize_window;

Resize_window:
    window->setGeometry( newGeometry );
}


void
TP_MainClass::slot_leftButtonReleased()
{
    // main window and playlist window
    if( checkAdjacentType( mainWindow->geometry(), playlistWindow->geometry() ) != TP::notAdjacent )
    {
        //qDebug() << "[TP_MainClass] main window is snapped to playlist window at position "
        //         << checkAdjacentType( mainWindow->geometry(), playlistWindow->geometry() );
        snapStatus [TP::mainWindow][TP::playlistWindow] = true;
        snapStatus [TP::playlistWindow][TP::mainWindow] = true;
        snapPosition_playlistWindow = playlistWindow->pos() - mainWindow->pos();
    }
    else
    {
        //qDebug("[TP_MainClass] main window desnapped to playlist window.");
        snapStatus [TP::mainWindow][TP::playlistWindow] = false;
        snapStatus [TP::playlistWindow][TP::mainWindow] = false;
        snapPosition_playlistWindow = { 0, 0 };
    }

    // Check other window pairs...
}


void
TP_MainClass::slot_connectFilelistWidget(TP_FileListWidget *I_FilelistWidget)
{
    qDebug() << "[SLOT] slot_connectFilelistWidget -- list's name is " << I_FilelistWidget->getListName();
    connect(I_FilelistWidget,   &TP_FileListWidget::itemDoubleClicked,
            this,               &TP_MainClass::slot_itemDoubleClicked);
}


void
TP_MainClass::slot_playButtonPushed()
{
    switch ( mediaPlayer->playbackState() )
    {
    case QMediaPlayer::PausedState :
        mediaPlayer->play();

        break;

    case QMediaPlayer::StoppedState :
        playItem( playlistWindow->getCurrentItem() );

        break;

    case QMediaPlayer::PlayingState :
        qDebug() << "[ERROR] state status inconsistent! Program will fix it manually.";
        mainWindow->setPlay();
        break;
    }
}


void
TP_MainClass::slot_nextButtonPushed()
{
    if( TP::Config().getPlayMode() == TP::shuffle )
        playItem( playlistWindow->getNextItem_shuffle() );
    else
        playItem( playlistWindow->getNextItem() );
}


void
TP_MainClass::slot_previousButtonPushed()
{
    if( TP::Config().getPlayMode() == TP::shuffle )
        playItem( playlistWindow->getPreviousItem_shuffle() );
    else
        playItem( playlistWindow->getPreviousItem() );
}


void
TP_MainClass::slot_playbackStateChanged( QMediaPlayer::PlaybackState newState )
{
    QString path {}, extension {};
    TP::SourceType sourceType { static_cast<TP::SourceType>( currentItem->data( TP::role_SourceType ).toInt() ) };

    if( sourceType == TP::singleFile )
    {
        path = currentItem->data( TP::role_URL ).value<QUrl>().toLocalFile();
        extension = TP::extension ( path );
    }

    switch ( newState )
    {
    case QMediaPlayer::PlayingState:
        qDebug() << "[Media Player] Playback state changed to PlayingState.";
        mainWindow->setPlay();
        mainWindow->setAudioInformation( currentItem );

        if( sourceType == TP::singleFile )
        {
            if( extension == QString( "flac" ) )
                mainWindow->setAlbumCover(
                            getCoverImageFromFLAC( path )
                            );
            else
                mainWindow->setAlbumCover(
                            getCoverImageFromID3V2( path )
                            );
        }

        playlistWindow->setCurrentItemBold();

        break;

    case QMediaPlayer::PausedState:
        qDebug() << "[Media Player] Playback state changed to PauseState.";
        mainWindow->setPause();

        break;

    case QMediaPlayer::StoppedState:
        qDebug() << "[Media Player] Playback state changed to StoppedState.";
        mainWindow->setStop();
        mainWindow->setAlbumCover( QImage {} );
        playlistWindow->unsetCurrentItemBold();

        break;
    }
}


void
TP_MainClass::slot_mediaStatusChanged ( QMediaPlayer::MediaStatus status )
{
    switch ( status )
    {
    case QMediaPlayer::NoMedia :
        qDebug("[Media Player] Media status changed to NoMedia.");
        break;

    case QMediaPlayer::LoadingMedia :
        qDebug("[Media Player] Media status changed to LoadingMedia.");
        break;

    case QMediaPlayer::LoadedMedia :
        qDebug("[Media Player] Media status changed to LoadedMedia.");
        break;

    case QMediaPlayer::StalledMedia :
        qDebug("[Media Player] Media status changed to StalledMedia.");
        break;

    case QMediaPlayer::BufferingMedia :
        qDebug("[Media Player] Media status changed to BufferingMedia.");
        break;

    case QMediaPlayer::BufferedMedia :
        qDebug("[Media Player] Media status changed to BufferedMedia.");
        break;

    case QMediaPlayer::EndOfMedia :
        qDebug("[Media Player] Media status changed to EndOfMedia.");

        switch( TP::Config().getPlayMode() )
        {
        case TP::singleTime :
            break;

        case TP::repeat :
            mediaPlayer->play();
            break;

        case TP::sequential :
            playItem( playlistWindow->getNextItem() );
            break;

        case TP::shuffle :
            playItem( playlistWindow->getPreviousItem() );
            break;
        }

        break;

    case QMediaPlayer::InvalidMedia :
        qDebug("[Media Player] Media status changed to InvalidMedia.");
        break;
    }
}


void
TP_MainClass::slot_changePlayingPosition( int second )
{
    mediaPlayer->setPosition( second * 1000 );
}


// *****************************************************************
// private
// *****************************************************************


void
TP_MainClass::initializeConnection()
{
    // Playerback control related
    connect(mediaPlayer,    &QMediaPlayer::positionChanged,
            mainWindow,     &TP_MainWindow::slot_updateDuration);
    connect(mediaPlayer,    &QMediaPlayer::playbackStateChanged,
            this,           &TP_MainClass::slot_playbackStateChanged);
    connect(mediaPlayer,    &QMediaPlayer::mediaStatusChanged,
            this,           &TP_MainClass::slot_mediaStatusChanged);

    connect(mainWindow,     &TP_MainWindow::signal_playButtonPushed,
            this,           &TP_MainClass::slot_playButtonPushed);
    connect(mainWindow,     &TP_MainWindow::signal_pauseButtonPushed,
            mediaPlayer,    &QMediaPlayer::pause);
    connect(mainWindow,     &TP_MainWindow::signal_stopButtonPushed,
            mediaPlayer,    &QMediaPlayer::stop);

    connect(mainWindow, &TP_MainWindow::signal_timeSliderPressed,
            this,       &TP_MainClass::slot_changePlayingPosition);

    // Source switching related
    connect(mainWindow,     &TP_MainWindow::signal_modeIsNotShuffle,
            playlistWindow, &TP_PlaylistWindow::slot_modeIsNotShuffle);
    connect(mainWindow,     &TP_MainWindow::signal_nextButtonPushed,
            this,           &TP_MainClass::slot_nextButtonPushed);
    connect(mainWindow,     &TP_MainWindow::signal_previousButtonPushed,
            this,           &TP_MainClass::slot_previousButtonPushed);

    // Volume control related
    connect(audioOutput,    &QAudioOutput::volumeChanged,
            mainWindow,     &TP_MainWindow::slot_changeVolumeSlider);
    connect(mainWindow,     &TP_MainWindow::signal_volumeSliderValueChanged,
            audioOutput,    &QAudioOutput::setVolume);

    // Windows minimizing & restoring related
    connect(mainWindow,     &TP_MainWindow::signal_minimizeWindow,
            this,           &TP_MainClass::slot_minimizeWindow);
    connect(mainWindow,     &TP_MainWindow::signal_restoreWindow,
            this,           &TP_MainClass::slot_restoreWindow);

    // Windows moving & resizing related
    connect(mainWindow,     &TP_MainWindow::signal_moveWindow,
            this,           &TP_MainClass::slot_moveWindow);
    connect(mainWindow,     &TP_MainWindow::signal_resizeWindow,
            this,           &TP_MainClass::slot_resizeWindow);
    connect(mainWindow,     &TP_MainWindow::signal_leftButtonReleased,
            this,           &TP_MainClass::slot_leftButtonReleased);

    connect(playlistWindow, &TP_PlaylistWindow::signal_moveWindow,
            this,           &TP_MainClass::slot_moveWindow);
    connect(playlistWindow, &TP_PlaylistWindow::signal_resizeWindow,
            this,           &TP_MainClass::slot_resizeWindow);
    connect(playlistWindow, &TP_PlaylistWindow::signal_leftButtonReleased,
            this,           &TP_MainClass::slot_leftButtonReleased);

    // Showing and hiding PlaylistWindow
    connect(playlistWindow, &TP_PlaylistWindow::signal_hidden,
            mainWindow,     &TP_MainWindow::slot_playlistWindowHidden);
    connect(playlistWindow, &TP_PlaylistWindow::signal_shown,
            mainWindow,     &TP_MainWindow::slot_playlistWindowShown);
    connect(mainWindow,     &TP_MainWindow::signal_openPlaylistWindow,
            playlistWindow, &TP_PlaylistWindow::show);
    connect(mainWindow,     &TP_MainWindow::signal_hidePlaylistWindow,
            playlistWindow, &TP_PlaylistWindow::hide);

    // Make PlaylistWindow be able to emit signal for connecting its FileListWidget
    connect(playlistWindow, &TP_PlaylistWindow::signal_newFilelistWidgetCreated,
            this,           &TP_MainClass::slot_connectFilelistWidget);
}

void
TP_MainClass::unsnapInvisibleWindows()
{
    if( !playlistWindow->isVisible() )
        for( unsigned i {}; i < 4; i++ )
        {
            snapStatus [i][TP::playlistWindow] = false;
            snapStatus [TP::playlistWindow][i] = false;
        }
}

TP::SnapType
TP_MainClass::checkSnapType( const QRect &geometry1, const QRect &geometry2 ) const
{
    // Intersect horizontally
    if( geometry1.right() >= geometry2.left() && geometry1.left() <= geometry2.right() )
    {
        if( std::abs( geometry1.top() - geometry2.bottom() ) <= TP::snapRange )
            return TP::toBottom;

        if( std::abs( geometry1.bottom() - geometry2.top() ) <= TP::snapRange)
            return TP::toTop;
    }

    // Intersect vertically
    if( geometry1.bottom() >= geometry2.top() && geometry1.top() <= geometry2.bottom() )
    {
        if( std::abs( geometry1.right() - geometry2.left() ) <= TP::snapRange)
            return TP::toLeft;

        if( std::abs( geometry1.left() - geometry2.right() ) <= TP::snapRange)
            return TP::toRight;
    }

    return TP::notAdjacent;
}

TP::SnapType
TP_MainClass::checkAdjacentType( const QRect &geometry1, const QRect &geometry2 ) const
{
    // Intersect horizontally
    if( geometry1.right() >= geometry2.left() && geometry1.left() <= geometry2.right() )
    {
        if( geometry1.top() - geometry2.bottom() == 1 )
            return TP::toBottom;

        if( geometry2.top() - geometry1.bottom() == 1 )
            return TP::toTop;
    }

    // Intersect vertically
    if( geometry1.bottom() >= geometry2.top() && geometry1.top() <= geometry2.bottom() )
    {
        if( geometry2.left() - geometry1.right() == 1 )
            return TP::toLeft;

        if( geometry1.left() - geometry2.right() == 1 )
            return TP::toRight;
    }

    return TP::notAdjacent;
}

bool
TP_MainClass::breadthFirstSearch( unsigned idx_Target ) const
{
    // The search always start from main window.
    unsigned idx_Current {};
    bool isVisited [4] {false};
    std::deque <unsigned> queue {};

    isVisited [ TP::mainWindow ] = true;
    queue.push_back( TP::mainWindow );

    while( !queue.empty() )
    {
        idx_Current = queue.front();
        queue.pop_front();

        for( unsigned i { 1 }; i < 4; i++ )
        {
            if( ! isVisited [i] && snapStatus [idx_Current][i] )
            {
                if( i == idx_Target )
                    return true;
                else
                {
                    isVisited [i] = true;
                    queue.push_back( i );
                }
             }
        }
    }

    return false;
}

void
TP_MainClass::playItem ( QListWidgetItem *I_item )
{
    switch ( I_item->data( TP::role_SourceType ).value<TP::SourceType>() )
    {
    case TP::singleFile :
        playFile( I_item );
        break;
    default:
        break;
    }
}

void
TP_MainClass::playFile ( QListWidgetItem *I_item )
{
    QUrl url { I_item->data( TP::role_URL ).value<QUrl>() };
    qDebug() << "Start playing local file URL: " << url;

    if( std::filesystem::exists( url.toLocalFile().toLocal8Bit().constData() ) )
    {
        playlistWindow->setCurrentItem( currentItem = I_item );
        mediaPlayer->setSource( url );
        mediaPlayer->play();
    }
}

QImage
TP_MainClass::getCoverImageFromFLAC( const QString &filePath )
{
#ifdef Q_OS_Windows
    TagLib::FLAC::File flacFile { filePath.toStdWString().c_str() };
#else
    TagLib::FLAC::File flacFile { filePath.toLocal8Bit().constData() };
#endif

    const TagLib::List <TagLib::FLAC::Picture *> &pictureList = flacFile.pictureList();
    if( pictureList.size() > 0 )
        return QImage::fromData(
                    QByteArray { pictureList[0]->data().data(), pictureList[0]->data().size() }
                    );

    return {};
}

QImage
TP_MainClass::getCoverImageFromID3V2( const QString &filePath )
{
#ifdef Q_OS_Windows
    TagLib::MPEG::File mpegFile { filePath.toStdWString().c_str() };
#else
    TagLib::MPEG::File mpegFile { filePath.toLocal8Bit().constData() };
#endif

    TagLib::ID3v2::Tag *id3v2Tag { nullptr };

    if( TP::extension( filePath ) == QString{ "MP3" } )
        id3v2Tag = mpegFile.ID3v2Tag();

    if( id3v2Tag )
    {
        TagLib::ID3v2::FrameList frameList { id3v2Tag->frameList("APIC") };
        TagLib::ID3v2::AttachedPictureFrame *pictureFrame =
            static_cast<TagLib::ID3v2::AttachedPictureFrame *>( frameList.front() );
        return QImage::fromData(
                    QByteArray { pictureFrame->picture().data(), pictureFrame->picture().size() }
                    );
    }
    return {};
}
