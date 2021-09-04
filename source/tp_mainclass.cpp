#include "tp_mainclass.h"

#include "tp_globalconst.h"
#include "tp_globalenum.h"

#include "tp_mainwindow.h"

#include "tp_playlistwindow.h"
#include "tp_filelistwidget.h"

#include <QAbstractButton>
#include <QApplication>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QListWidgetItem>
#include <QMediaDevices>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>
#include <QWindow>

#include <cmath>
#include <deque>
#include <filesystem>

TP_MainClass::TP_MainClass() :
    QObject { nullptr }
  , mainWindow { new TP_MainWindow {} }
  , playlistWindow { new TP_PlaylistWindow {} }
  , audioOutput { new QAudioOutput { this } }
  , mediaPlayer { new QMediaPlayer { this } }
  , currentItem {}
  , snapStatus { TP::notSnapped }
  , snapPosition_playlistWindow {}
{
    mediaPlayer->setAudioOutput(audioOutput);
    qDebug()<< "Current audio output device is "<< mediaPlayer->audioOutput()->device().description();

    initializeConnection();

    mainWindow->show();
    mainWindow->setVolumeSliderValue( 50 );

    playlistWindow->initializePlaylist();       // Must be executed before showing
    playlistWindow->show();
}

TP_MainClass::~TP_MainClass()
{
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
    if ( !mediaPlayer->isAvailable() )
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
    QPoint globalPositionBottomLeft_PlaylistWindow = mainWindow->geometry().bottomLeft();
    qDebug() << "globalPositionBottomLeft_PlaylistWindow: " << globalPositionBottomLeft_PlaylistWindow;
    playlistWindow->move( globalPositionBottomLeft_PlaylistWindow );
}


// *****************************************************************
// private slots:
// *****************************************************************

void
TP_MainClass::slot_playButtonPushed()
{
    switch ( mediaPlayer->playbackState() )
    {
    case QMediaPlayer::PausedState :
        mediaPlayer->play();
        break;

    case QMediaPlayer::StoppedState :
        // pending implementation: get a file to play from the playlist widget
        break;

    case QMediaPlayer::PlayingState :
        qDebug() << "[FATAL ERROR] state status inconsistent! Program will fix it manually.";
        mainWindow->setPlay();
        break;
    }
}

void
TP_MainClass::slot_playItem( QListWidgetItem *I_listWidgetItem )
{
    switch ( I_listWidgetItem->data( TP::role_SourceType ).value<TP::SourceType>() )
    {
    case TP::single :
        playFile( I_listWidgetItem );
        break;
    default:
        break;
    }
}

void
TP_MainClass::slot_moveWindow( QWidget *window, QRect newGeometry )
{
    // A window can only be snapped once vertically and horizontally, respectively.
    bool isVerticallySnapped = false;
    bool isHorizontallySnapped = false;

    unsigned typeOfWindow {};

    if( window == mainWindow )
        typeOfWindow = TP::mainWindow;
    if( window == playlistWindow )
        typeOfWindow = TP::playlistWindow;

    if( typeOfWindow == TP::mainWindow )
    {
        // ============================== Snapping to playlist window ==============================

        if( playlistWindow->isVisible() && ! breadthFirstSearch( TP::playlistWindow ) )
        {
            // ------------------ Step 1 (edge sticks to edge) ------------------
            TP::SnapType snapType { checkSnapType( newGeometry, playlistWindow->geometry(), TP::snapRange ) };

            switch( snapType )
            {
            case TP::toBottom :
                newGeometry.moveTop( playlistWindow->geometry().bottom() + 1 );
                isVerticallySnapped = true;
                goto MOVE_Main_window_snaps_to_playlist_window_edge;

            case TP::toTop :
                newGeometry.moveBottom( playlistWindow->geometry().top() - 1 );
                isVerticallySnapped = true;
                goto MOVE_Main_window_snaps_to_playlist_window_edge;

            case TP::toRight :
                newGeometry.moveLeft( playlistWindow->geometry().right() + 1 );
                isHorizontallySnapped = true;
                goto MOVE_Main_window_snaps_to_playlist_window_edge;

            case TP::toLeft :
                newGeometry.moveRight( playlistWindow->geometry().left() - 1 );
                isHorizontallySnapped = true;
                goto MOVE_Main_window_snaps_to_playlist_window_edge;

            case TP::notAdjacent :
                snapStatus [TP::mainWindow][TP::playlistWindow] = TP::notSnapped;
                snapStatus [TP::playlistWindow][TP::mainWindow] = TP::notSnapped;
                goto MOVE_End_of_main_window_snaps_to_playlist_window;
            }

            // ---------- Step 2 (edge alignment in the vertical direction) ----------

MOVE_Main_window_snaps_to_playlist_window_edge:

            snapStatus [TP::mainWindow][TP::playlistWindow] = TP::pending;
            snapStatus [TP::playlistWindow][TP::mainWindow] = TP::pending;

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

MOVE_End_of_main_window_snaps_to_playlist_window:   int dummy;
/*
        // ============================== Snapping to equalizer window ==============================

        if( equalizerWindow->isVisible() && snapStatus_equalizerWindow != TP::snapped )
        {
            // If main window has perfectly snapped to some window before,
            // it cannot be moved anymore.
            // However, it is still necessary to detect the snap status
            // near the borders of main window.
            if( isVerticallySnapped && isHorizontallySnapped )
            {
                // Detect if the window is near main window.
                goto MOVE_End_of_main_window_snaps_to_equalizer_window;
            }


        }
MOVE_End_of_main_window_snaps_to_equalizer_window:
*/
    }   // if ( window == mainWindow )
    else //********************************** if ( window != mainWindow ) **********************************
    {
        // ============================== Snapping to main window ==============================
        {
            TP::SnapType snapType { checkSnapType( newGeometry, mainWindow->geometry(), TP::snapRange ) };
            // --------------------- Step 1 (edge sticks to edge) ---------------------

            switch( snapType )
            {
            case TP::toBottom :
                newGeometry.moveTop( mainWindow->geometry().bottom() + 1 );
                isVerticallySnapped = true;
                goto MOVE_Snap_to_main_window_edge;

            case TP::toTop :
                newGeometry.moveBottom( mainWindow->geometry().top() - 1 );
                isVerticallySnapped = true;
                goto MOVE_Snap_to_main_window_edge;

            case TP::toRight :
                newGeometry.moveLeft( mainWindow->geometry().right() + 1 );
                isHorizontallySnapped = true;
                goto MOVE_Snap_to_main_window_edge;

            case TP::toLeft :
                newGeometry.moveRight( mainWindow->geometry().left() - 1 );
                isHorizontallySnapped = true;
                goto MOVE_Snap_to_main_window_edge;

            case TP::notAdjacent :
                snapStatus [typeOfWindow][TP::mainWindow] = TP::notSnapped;
                snapStatus [TP::mainWindow][typeOfWindow] = TP::notSnapped;
                goto MOVE_End_of_snap_to_main_window_edge;
            }

            // ---------- Step 2 (edge alignment in the vertical direction) ----------

MOVE_Snap_to_main_window_edge:

            snapStatus [typeOfWindow][TP::mainWindow] = TP::pending;
            snapStatus [TP::mainWindow][typeOfWindow] = TP::pending;

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
    }       // else

MOVE_End_of_snap_to_main_window_edge:


// ==================== Move window ====================
Move_window:

    window->setGeometry( newGeometry );
    if( window == mainWindow )
        // If window is main window, move the snapped window(s) as well
    {
        unsnapInvisibleWindows();

        // Move playlist window
        if ( snapStatus [TP::mainWindow][TP::playlistWindow] == TP::snapped )
            playlistWindow->move( newGeometry.topLeft() + snapPosition_playlistWindow );

        // Move other windows...
    }
}

void
TP_MainClass::slot_titleBarReleased()
{
    for( unsigned i {}; i < 4; i++ )
        for( unsigned j {}; j < 4; j++ )
            if( snapStatus [i][j] == TP::pending )
                snapStatus [i][j] = TP::snapped;

    if( breadthFirstSearch( TP::playlistWindow ) )
        snapPosition_playlistWindow = playlistWindow->pos() - mainWindow->pos();

    // Check other windows...
}

void
TP_MainClass::slot_resizeWindow( QWidget *window, QRect newGeometry, TP::ResizeType resizeType )
{/*
    // While resizing, a window can only be snapped once.
    bool isSnapped = false;
    TP::WindowType  typeOfWindow {};
    TP::SnapType    snapType;

    if( window == mainWindow )
        typeOfWindow = TP::mainWindow;
    if( window == playlistWindow )
        typeOfWindow = TP::playlistWindow;

    // ============================== Snapping to main window ==============================

    if( window != mainWindow )
    {
        switch ( resizeType )
        {
        case TP::left:
            if
            (
                (
                    newGeometry.top() < mainWindow->geometry().bottom()
                    ||
                    newGeometry.bottom() > mainWindow->geometry().top()
                )
                &&
                    std::abs( newGeometry.left() - playlistWindow->geometry().right() )
                    < TP::snapBorderSize
            )
            {
                newGeometry.setLeft( mainWindow->geometry().right() + 1 );
                isSnapped = true;
                break;
            }

        case TP::right:
            if
            (
                (
                    newGeometry.top() < mainWindow->geometry().bottom()
                    ||
                    newGeometry.bottom() > mainWindow->geometry().top()
                )
                &&
                    std::abs( newGeometry.right() - playlistWindow->geometry().left() )
                    < TP::snapBorderSize
            )
            {
                newGeometry.setRight( mainWindow->geometry().left() - 1 );
                isSnapped = true;
                break;
            }

        case TP::bottom:
            if
            (
                (
                    newGeometry.left() < mainWindow->geometry().right()
                    ||
                    newGeometry.right() > mainWindow->geometry().left()
                )
                &&
                    std::abs( newGeometry.bottom() - playlistWindow->geometry().top() )
                    < TP::snapBorderSize
            )
            {
                newGeometry.setBottom( mainWindow->geometry().top() - 1 );
                isSnapped = true;
                break;
            }
        default:
                break;
        }       // switch ( resizeType )

        switch( typeOfWindow )
        {
        case TP::playlistWindow :
            //if
        }
    }       //if( window != mainWindow )*/
}

void
TP_MainClass::slot_connectFilelistWidget(TP_FileListWidget *I_FilelistWidget)
{
    qDebug() << "[SLOT] slot_connectFilelistWidget -- list's name is " << I_FilelistWidget->getListName();
    connect(I_FilelistWidget,   &TP_FileListWidget::itemDoubleClicked,
            this,               &TP_MainClass::slot_playItem);
}

void
TP_MainClass::slot_playbackStateChanged( QMediaPlayer::PlaybackState newState )
{
    switch ( newState )
    {
    case QMediaPlayer::PlayingState:
        qDebug() << "[SLOT] Media player status changed to PlayingState.";

        // Modify main window
        mainWindow->setPlay();
        mainWindow->setAudioInformation( currentItem );

        // Modify playlist window
        playlistWindow->setBold( currentItem );

        break;


    case QMediaPlayer::PausedState:
        qDebug() << "[SLOT] Media player status changed to PauseState.";

        // Modify main window
        mainWindow->setPause();

        break;


    case QMediaPlayer::StoppedState:
        qDebug() << "[SLOT] Media player status changed to StoppedState.";

        // Modify main window
        mainWindow->setStop();
        // Modify playlist window
        playlistWindow->unsetAllBolds();

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

    connect(mainWindow,     &TP_MainWindow::signal_playButtonPushed,
            this,           &TP_MainClass::slot_playButtonPushed);
    connect(mainWindow,     &TP_MainWindow::signal_pauseButtonPushed,
            mediaPlayer,    &QMediaPlayer::pause);
    connect(mainWindow,     &TP_MainWindow::signal_stopButtonPushed,
            mediaPlayer,    &QMediaPlayer::stop);

    connect(mainWindow, &TP_MainWindow::signal_timeSliderPressed,
            this,       &TP_MainClass::slot_changePlayingPosition);

    // Volume control related
    connect(audioOutput,    &QAudioOutput::volumeChanged,
            mainWindow,     &TP_MainWindow::slot_changeVolumeSlider);
    connect(mainWindow,     &TP_MainWindow::signal_volumeSliderValueChanged,
            audioOutput,    &QAudioOutput::setVolume);

    // Title bar related
    connect(mainWindow,     &TP_MainWindow::signal_moveWindow,
            this,           &TP_MainClass::slot_moveWindow);
    connect(mainWindow,     &TP_MainWindow::signal_titleBarReleased,
            this,           &TP_MainClass::slot_titleBarReleased);
    connect(playlistWindow, &TP_PlaylistWindow::signal_moveWindow,
            this,           &TP_MainClass::slot_moveWindow);
    connect(playlistWindow, &TP_PlaylistWindow::signal_titleBarReleased,
            this,           &TP_MainClass::slot_titleBarReleased);

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
    {
        snapStatus [TP::mainWindow][TP::playlistWindow] = TP::notSnapped;
        snapStatus [TP::playlistWindow][TP::mainWindow] = TP::notSnapped;
    }
}

TP::SnapType
TP_MainClass::checkSnapType( const QRect &geometry1, const QRect &geometry2, unsigned maximumGap ) const
{
    // Intersect horizontally
    if( geometry1.right() >= geometry2.left() && geometry1.left() <= geometry2.right() )
    {
        if( std::abs( geometry1.top() - geometry2.bottom() ) <= maximumGap )
            return TP::toBottom;

        if( std::abs( geometry1.bottom() - geometry2.top() ) <= maximumGap)
            return TP::toTop;
    }

    // Intersect vertically
    if( geometry1.bottom() >= geometry2.top() && geometry1.top() <= geometry2.bottom() )
    {
        if( std::abs( geometry1.right() - geometry2.left() ) <= maximumGap)
            return TP::toLeft;

        if( std::abs( geometry1.left() - geometry2.right() ) <= maximumGap)
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
            if( ! isVisited [i] && snapStatus [idx_Current][i] == TP::snapped )
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

// Play audio file
void
TP_MainClass::playFile( QListWidgetItem *I_listWidgetItem )
{
    QUrl url { I_listWidgetItem->data( TP::role_URL ).value<QUrl>() };
    qDebug() << "Start playing local file URL: " << url;

    if( std::filesystem::exists( url.toLocalFile().toLocal8Bit().constData() ) )
        currentItem = *I_listWidgetItem;
    else
        return;

    mediaPlayer->setSource( url );
    mediaPlayer->play();
}
