﻿#include "tp_mainclass.h"

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
    /********************************** Note **********************************
     *  QRect::bottom() and QRect::right(), as well as QRect::bottomRight()
     *  are not recommended to use since the result is not accurate.
     *  Check official Qt documentation for details.
     **************************************************************************/

    // A window can only be snapped once vertically and horizontally, respectively.
    bool isVerticallySnapped = false;
    bool isHorizontallySnapped = false;

    // Snap to main window
    if( window != mainWindow )
    {
        // Top edge snap to main window's bottom edge
        if(
                std::abs( mainWindow->geometry().top() + mainWindow->geometry().height() - newGeometry.top() )
                <
                TP::snapBorderSize
                )
        {
            newGeometry.moveTop( mainWindow->geometry().top() + mainWindow->geometry().height() + 1 );
            isVerticallySnapped = true;
            goto EndOfWindow;
        }

        // Bottom edge snap to main window's top edge
        if(
                std::abs( newGeometry.top() + newGeometry.height() - mainWindow->geometry().top() )
                <
                TP::snapBorderSize
                )
        {
            newGeometry.moveBottom( mainWindow->geometry().top() - 1 );
            isVerticallySnapped = true;
            goto EndOfWindow;
        }

        // Left edge snap to main window's right edge
        if(
                std::abs( mainWindow->geometry().left() + mainWindow->geometry().width() - newGeometry.left() )
                <
                TP::snapBorderSize
                )
        {
            newGeometry.moveLeft( mainWindow->geometry().left() + mainWindow->geometry().width() + 1 );
            isHorizontallySnapped = true;
            goto EndOfWindow;
        }

        // Right edge snap to main window's left edge
        if(
                std::abs( newGeometry.left() + newGeometry.width() - mainWindow->geometry().left() )
                <
                TP::snapBorderSize
                )
        {
            newGeometry.moveRight( mainWindow->geometry().left() - 1 );
            isHorizontallySnapped = true;
            goto EndOfWindow;
        }

    }

EndOfWindow:

    // Snap to playlist window
    if( window != playlistWindow && playlistWindow->isVisible() )
    {
        // Top edge snap to playlist window's bottom edge

        // Bottom edge snap to playlist window's top edge

        // Left edge snap to playlist window's right edge

        // Right edge snap to playlist window's left edge
    }

    window->setGeometry( newGeometry );
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

    // Snapping window while moving
    connect(mainWindow, &TP_MainWindow::signal_moveWindow,
            this,       &TP_MainClass::slot_moveWindow);

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
