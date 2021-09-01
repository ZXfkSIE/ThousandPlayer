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
#include <filesystem>

TP_MainClass::TP_MainClass() :
    QObject { nullptr }
  , mainWindow { new TP_MainWindow {} }
  , playlistWindow { new TP_PlaylistWindow {} }
  , audioOutput { new QAudioOutput { this } }
  , mediaPlayer { new QMediaPlayer { this } }
  , currentItem {}
  , b_isSnapped_playlistWindow { false }
  , snappingPosition_playlistWindow {}
{
    mediaPlayer->setAudioOutput(audioOutput);
    qDebug()<< "Current audio output device is "<< mediaPlayer->audioOutput()->device().description();

    qreal linearVolume =
            QAudio::convertVolume(0.5,
                                  QAudio::LogarithmicVolumeScale,
                                  QAudio::LinearVolumeScale);
    audioOutput->setVolume( qRound(linearVolume * 100) );

    initializeConnection();

    mainWindow->show();

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
    if ( !mediaPlayer->isAvailable() )
        msgBox_ServiceNotAvailable.exec();
}

void
TP_MainClass::slot_initializePosition()
{
    QPoint globalPositionBottomLeft_PlaylistWindow = mainWindow->geometry().bottomLeft();
    qDebug() << "globalPositionBottomLeft_PlaylistWindow: " << globalPositionBottomLeft_PlaylistWindow;
    playlistWindow->move( globalPositionBottomLeft_PlaylistWindow );
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

    int typeOfWindow {};

    if( window == mainWindow )
        typeOfWindow = TP::mainWindow;
    if( window == playlistWindow )
        typeOfWindow = TP::playlistWindow;

// ==================== Adjust newGeometry ====================

    // Snapping to main window

    if( window != mainWindow )
    {
        // Top edge snaps to main window's bottom edge
        if(
                std::abs( newGeometry.top() - mainWindow->geometry().bottom() )
                < TP::snapBorderSize
                )
        {
            newGeometry.moveTop( mainWindow->geometry().bottom() + 1 );
            isVerticallySnapped = true;
            goto Snapped_to_main_window;
        }

        // Bottom edge snaps to main window's top edge
        if(
                std::abs( newGeometry.bottom() - mainWindow->geometry().top() )
                < TP::snapBorderSize
                )
        {
            newGeometry.moveBottom( mainWindow->geometry().top() - 1 );
            isVerticallySnapped = true;
            goto Snapped_to_main_window;
        }

        // Left edge snasp to main window's right edge
        if(
                std::abs( newGeometry.left() - mainWindow->geometry().right() )
                < TP::snapBorderSize
                )
        {
            newGeometry.moveLeft( mainWindow->geometry().right() + 1 );
            isHorizontallySnapped = true;
            goto Snapped_to_main_window;
        }

        // Right edge snaps to main window's left edge
        if(
                std::abs( newGeometry.right() - mainWindow->geometry().left() )
                < TP::snapBorderSize
                )
        {
            newGeometry.moveRight( mainWindow->geometry().left() - 1 );
            isHorizontallySnapped = true;
            goto Snapped_to_main_window;
        }

        // This window does not snap to main window

        switch ( typeOfWindow )
        {
        case TP::playlistWindow :
            b_isSnapped_playlistWindow = TP::notSnapped;
            snappingPosition_playlistWindow = QPoint(0, 0);
            break;

        // case TP::equalizerWindow :

        default:
            break;
        }

        goto Not_snapped_to_main_window;

    }       // if( window != mainWindow )

Snapped_to_main_window:
    switch ( typeOfWindow )
    {
    case TP::playlistWindow :
        b_isSnapped_playlistWindow = TP::pending;
        snappingPosition_playlistWindow = window->pos() - mainWindow->pos();
        break;

    // case TP::equalizerWindow :

    default:
        break;
    }

Not_snapped_to_main_window:

    // Snapping to playlist window

    if( window != playlistWindow && playlistWindow->isVisible() )
    {
        // If it is main window and the playlist window has already been snapped to it, jump.
        if( window == mainWindow && b_isSnapped_playlistWindow == TP::snapped )
            goto Not_snapped_to_playlist_window;

        // Top edge snaps to playlist window's bottom edge
        if(
                std::abs( newGeometry.top() - playlistWindow->geometry().bottom() )
                < TP::snapBorderSize
                && !isVerticallySnapped
                )
        {
            newGeometry.moveTop( playlistWindow->geometry().bottom() + 1 );
            isVerticallySnapped = true;
            goto Snapped_to_playlist_window;
        }

        // Bottom edge snaps to playlist window's top edge
        if(
                std::abs( newGeometry.bottom() - playlistWindow->geometry().top() )
                < TP::snapBorderSize
                && !isVerticallySnapped
                )
        {
            newGeometry.moveBottom( playlistWindow->geometry().top() - 1 );
            isVerticallySnapped = true;
            goto Snapped_to_playlist_window;
        }

        // Left edge snaps to playlist window's right edge
        if(
                std::abs( newGeometry.left() - playlistWindow->geometry().right() )
                < TP::snapBorderSize
                && !isHorizontallySnapped
                )
        {
            newGeometry.moveLeft( playlistWindow->geometry().right() + 1 );
            isHorizontallySnapped = true;
            goto Snapped_to_playlist_window;
        }

        // Right edge snaps to playlist window's left edge
        if(
                std::abs( newGeometry.right() - playlistWindow->geometry().left() )
                < TP::snapBorderSize
                && !isHorizontallySnapped
                )
        {
            newGeometry.moveRight( playlistWindow->geometry().left() - 1 );
            isHorizontallySnapped = true;
            goto Snapped_to_playlist_window;
        }

        if( window == mainWindow )
            // Playlist window does not snap to main window
        {
            b_isSnapped_playlistWindow = TP::notSnapped;
            snappingPosition_playlistWindow = QPoint(0, 0);
        }

        goto Not_snapped_to_playlist_window;

    }       // if( window != playlistWindow && playlistWindow->isVisible() )

Snapped_to_playlist_window:
    if( window == mainWindow )
    {
        b_isSnapped_playlistWindow = TP::pending;
        snappingPosition_playlistWindow = playlistWindow->pos() - window->pos();
    }

Not_snapped_to_playlist_window:
    if( isVerticallySnapped && isHorizontallySnapped )
        goto Move_window;

// ==================== Move window ====================
Move_window:

    window->setGeometry( newGeometry );
    if( window == mainWindow )
        // If window is main window, move the snapped window(s) as well
    {
        unsnapInvisibleWindows();

        if ( b_isSnapped_playlistWindow == TP::snapped && playlistWindow->isVisible() )
            playlistWindow->move( newGeometry.topLeft() + snappingPosition_playlistWindow );
        /*
        if ( b_isSnapped [ TP::equalizerWindow ] == TP::snapped )

        if ( b_isSnapped [ TP::lyricsWindow ] == TP::snapped )

        */
    }
}

void
TP_MainClass::slot_titleBarReleased()
{
    if( b_isSnapped_playlistWindow == TP::pending )
        b_isSnapped_playlistWindow = TP::snapped;
}

// *****************************************************************
// private slots:
// *****************************************************************

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

// *****************************************************************
// private
// *****************************************************************

void
TP_MainClass::initializeConnection()
{
    // Connections about media player control
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
    connect(playlistWindow, &TP_PlaylistWindow::signal_Hidden,
            mainWindow,     &TP_MainWindow::slot_playlistWindowHidden);
    connect(playlistWindow, &TP_PlaylistWindow::signal_Shown,
            mainWindow,     &TP_MainWindow::slot_playlistWindowShown);
    connect(mainWindow,     &TP_MainWindow::signal_openPlaylistWindow,
            playlistWindow, &TP_PlaylistWindow::show);
    connect(mainWindow,     &TP_MainWindow::signal_hidePlaylistWindow,
            playlistWindow, &TP_PlaylistWindow::hide);

    // Make PlaylistWindow be able to emit signal for connecting its FileListWidget
    connect(playlistWindow, &TP_PlaylistWindow::signal_NewFilelistWidgetCreated,
            this,           &TP_MainClass::slot_connectFilelistWidget);
}

void
TP_MainClass::unsnapInvisibleWindows()
{
    if( !playlistWindow->isVisible() )
        b_isSnapped_playlistWindow = TP::notSnapped;
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
