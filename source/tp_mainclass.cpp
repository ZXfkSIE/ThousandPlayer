﻿#include "tp_mainclass.h"

#include "tp_globalfunction.h"
#include "tp_globalvariable.h"

#include "tp_configwindow.h"
#include "tp_lyricswindow.h"
#include "tp_mainwindow.h"
#include "tp_playlistwindow.h"

#include <QAbstractButton>
#include <QApplication>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>
#include <QTimer>
#include <QWindow>

#include <cmath>
#include <filesystem>
#include <queue>

// Headers of TagLib
#include <attachedpictureframe.h>
#include <fileref.h>
#include <flacfile.h>
#include <id3v2frame.h>
#include <id3v2tag.h>
#include <mp4file.h>
#include <mp4tag.h>
#include <mpegfile.h>
#include <vorbisfile.h>
#include <xiphcomment.h>

TP_MainClass::TP_MainClass() :
    QObject                     { nullptr }
  , mainWindow                  { new TP_MainWindow {} }
  , playlistWindow              { new TP_PlaylistWindow {} }
  , configWindow                { new TP_ConfigWindow {} }
  , lyricsWindow                { new TP_LyricsWindow {} }
  , b_isPlaylistWindowVisible   { true }
  , b_isLyricsWindowVisible     { true }
  , audioOutput                 { new QAudioOutput { this } }
  , mediaPlayer                 { new QMediaPlayer { this } }
  , b_isStopInterrupting        { false }
  , snapStatus                  {}
  , snapPosition_playlistWindow {}
  , snapPosition_lyricsWindow   {}
{
    mediaPlayer->setAudioOutput( audioOutput );
    qDebug() << "[Audio Output] Default audio device:" << mediaPlayer->audioOutput()->device().description();
    TP::playbackState() = mediaPlayer->playbackState();

    initializeConnection();

    mainWindow->initializeVolume();
}


TP_MainClass::~TP_MainClass()
{
    // Get current screen
    auto *mainWindowScreen { QApplication::screenAt( mainWindow->pos() ) };
    auto mainWindowGeometry { mainWindow->geometry() };
    mainWindowGeometry.moveTopLeft( mainWindow->pos() - mainWindowScreen->geometry().topLeft() );
    TP::config().setMainWindowGeometry( mainWindowGeometry );

    if( playlistWindow->isVisible() || b_isPlaylistWindowVisible )
    {
        auto *playlistWindowScreen { QApplication::screenAt( playlistWindow->pos() ) };
        auto playlistWindowGeometry { playlistWindow->geometry() };
        playlistWindowGeometry.moveTopLeft( playlistWindow->pos() - playlistWindowScreen->geometry().topLeft() );
        TP::config().setPlaylistWindowGeometry( playlistWindowGeometry );
        TP::config().setPlaylistWindowShown( true );
    }
    else
    {
        TP::config().setPlaylistWindowGeometry();
        TP::config().setPlaylistWindowShown( false );
    }

    if( lyricsWindow->isVisible() || b_isLyricsWindowVisible )
    {
        auto *lyricsWindowScreen { QApplication::screenAt( lyricsWindow->pos() ) };
        auto lyricsWindowGeometry { lyricsWindow->geometry() };
        lyricsWindowGeometry.moveTopLeft( lyricsWindow->pos() - lyricsWindowScreen->geometry().topLeft() );
        TP::config().setLyricsWindowGeometry( lyricsWindowGeometry );
        TP::config().setLyricsWindowShown( true );
    }
    else
    {
        TP::config().setLyricsWindowGeometry();
        TP::config().setLyricsWindowShown( false );
    }

    delete mainWindow;
    delete configWindow;
    delete lyricsWindow;
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
                    tr( "Not Available" ),
                    tr( "QMediaPlayer is not supported on this platform.\n"
                        "The program will quit now." ),
                    QMessageBox::NoButton );
        msgBox_ServiceNotAvailable.addButton(
                    tr( "Exit" ),
                    QMessageBox::AcceptRole );
        connect( &msgBox_ServiceNotAvailable, &QMessageBox::buttonClicked, qApp, &QApplication::quit );

        msgBox_ServiceNotAvailable.exec();
    }
}


void
TP_MainClass::slot_initializePosition()
{
    auto *currentScreen { QApplication::screenAt( QCursor::pos() ) };

    auto mainWindowGeometry { TP::config().getMainWindowGeometry() };
    mainWindowGeometry.moveTopLeft( mainWindowGeometry.topLeft() + currentScreen->geometry().topLeft() );
    mainWindow->setGeometry( mainWindowGeometry );
    mainWindow->show();
    mainWindow->raise();
    mainWindow->activateWindow();

    auto playlistWindowGeometry { TP::config().getPlaylistWindowGeometry() };
    playlistWindowGeometry.moveTopLeft( playlistWindowGeometry.topLeft() + currentScreen->geometry().topLeft() );
    playlistWindow->setGeometry( playlistWindowGeometry );
    if ( TP::config().isPlaylistWindowShown() )
    {
        playlistWindow->show();
        playlistWindow->raise();
        playlistWindow->activateWindow();
    }
    else
        mainWindow->slot_playlistWindowHidden();

    auto lyricsWindowGeometry { TP::config().getLyricsWindowGeometry() };
    lyricsWindowGeometry.moveTopLeft( lyricsWindowGeometry.topLeft() + currentScreen->geometry().topLeft() );
    lyricsWindow->setGeometry( lyricsWindowGeometry );
    if ( TP::config().isLyricsWindowShown() )
    {
        lyricsWindow->show();
        lyricsWindow->raise();
        lyricsWindow->activateWindow();
    }
    else
        mainWindow->slot_lyricsWindowHidden();

    slot_refreshSnapStatus();

    playlistWindow->initializePlaylist();   // Must be executed after the initialization of position
}

// *****************************************************************
// private slots:
// *****************************************************************

void
TP_MainClass::slot_exitProgram()
{
    // Save LRC file
    if( ! lyricsWindow->saveLyricsFileBeforeQuit() )
        return;

    QApplication::quit();
}

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

    if( lyricsWindow->isVisible() )
    {
        b_isLyricsWindowVisible = true;
        lyricsWindow->hide();
    }
    else
        b_isLyricsWindowVisible = false;
}


void
TP_MainClass::slot_restoreWindow()
{
    if( b_isPlaylistWindowVisible )
    {
        playlistWindow->show();
        playlistWindow->raise();
    }

    if( b_isLyricsWindowVisible )
    {
        lyricsWindow->show();
        lyricsWindow->raise();
    }

    slot_refreshSnapStatus();
}


void
TP_MainClass::slot_moveWindow( QWidget *I_window, QRect I_geometry )
{
    // A window can only be snapped (or aligned) once at vertical and horizontal direction, respectively.
    bool isVerticallySnapped    {};
    bool isHorizontallySnapped  {};

    // ============================== Snapping to main window ==============================

    if ( I_window != mainWindow )
    {
        SnapType snapType { checkSnapType( I_geometry, mainWindow->geometry() ) };

        // --------------------- Step 1 (edge sticks to edge) ---------------------

        switch( snapType )
        {
        case SnapType::ToBottom :
            I_geometry.moveTop( mainWindow->geometry().bottom() + 1 );
            isVerticallySnapped = true;
            break;

        case SnapType::ToTop :
            I_geometry.moveBottom( mainWindow->geometry().top() - 1 );
            isVerticallySnapped = true;
            break;

        case SnapType::ToRight :
            I_geometry.moveLeft( mainWindow->geometry().right() + 1 );
            isHorizontallySnapped = true;
            break;

        case SnapType::ToLeft :
            I_geometry.moveRight( mainWindow->geometry().left() - 1 );
            isHorizontallySnapped = true;
            break;

        case SnapType::NonSnap :
            goto MOVE_End_of_snap_to_main_window;
        }

        // ---------- Step 2 (edge alignment in the vertical direction) ----------

        switch( snapType )
        {
        case SnapType::ToTop :
        case SnapType::ToBottom :

            // Left edge alignment
            if( std::abs( I_geometry.left() - mainWindow->geometry().left() )
                    < TP::snapRange )
            {
                I_geometry.moveLeft( mainWindow->geometry().left() );
                isHorizontallySnapped = true;
                break;
            }

            // Right edge alignment
            if( std::abs( I_geometry.right() - mainWindow->geometry().right() )
                    < TP::snapRange )
            {
                I_geometry.moveRight( mainWindow->geometry().right() );
                isHorizontallySnapped = true;
                break;
            }

            break;

        case SnapType::ToLeft :
        case SnapType::ToRight :

            // Top edge alignment
            if( std::abs( I_geometry.top() - mainWindow->geometry().top() )
                    < TP::snapRange )
            {
                I_geometry.moveTop( mainWindow->geometry().top() );
                isVerticallySnapped = true;
                break;
            }

            // Bottom edge alignment
            if( std::abs( I_geometry.bottom() - mainWindow->geometry().bottom() )
                    < TP::snapRange )
            {
                I_geometry.moveBottom( mainWindow->geometry().bottom() );
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
        goto MOVE_setGeometry;


    // ============================== Snapping to playlist window ==============================

    if( I_window != playlistWindow && playlistWindow->isVisible() )
    {
        // If it is main window and playlist window has already been snapped to it, jump.
        if( I_window == mainWindow && breadthFirstSearch( TP::playlistWindow ) )
            goto MOVE_End_of_snap_to_playlist_window;

        // ------------------ Step 1 (edge sticks to edge) ------------------

        SnapType snapType { checkSnapType( I_geometry, playlistWindow->geometry() ) };

        switch( snapType )
        {
        case SnapType::ToBottom :
            I_geometry.moveTop( playlistWindow->geometry().bottom() + 1 );
            isVerticallySnapped = true;
            break;

        case SnapType::ToTop :
            I_geometry.moveBottom( playlistWindow->geometry().top() - 1 );
            isVerticallySnapped = true;
            break;

        case SnapType::ToRight :
            I_geometry.moveLeft( playlistWindow->geometry().right() + 1 );
            isHorizontallySnapped = true;
            break;

        case SnapType::ToLeft :
            I_geometry.moveRight( playlistWindow->geometry().left() - 1 );
            isHorizontallySnapped = true;
            break;

        case SnapType::NonSnap :
            goto MOVE_End_of_snap_to_playlist_window;
        }

        // ---------- Step 2 (edge alignment in the vertical direction) ----------

        switch( snapType )
        {
        case SnapType::ToTop :
        case SnapType::ToBottom :

            // Left edge alignment
            if( std::abs( I_geometry.left() - playlistWindow->geometry().left() )
                    <= TP::snapRange
                && ! isHorizontallySnapped )
            {
                I_geometry.moveLeft( playlistWindow->geometry().left() );
                isHorizontallySnapped = true;
                break;
            }

            // Right edge alignment
            if( std::abs( I_geometry.right() - playlistWindow->geometry().right() )
                    <= TP::snapRange
                && ! isHorizontallySnapped )
            {
                I_geometry.moveRight( playlistWindow->geometry().right() );
                isHorizontallySnapped = true;
                break;
            }

            break;

        case SnapType::ToLeft :
        case SnapType::ToRight :

            // Top edge alignment
            if( std::abs( I_geometry.top() - playlistWindow->geometry().top() )
                    <= TP::snapRange
                && ! isVerticallySnapped )
            {
                I_geometry.moveTop( playlistWindow->geometry().top() );
                isVerticallySnapped = true;
                break;
            }

            // Bottom edge alignment
            if( std::abs( I_geometry.bottom() - playlistWindow->geometry().bottom() )
                    <= TP::snapRange
                && ! isVerticallySnapped )
            {
                I_geometry.moveBottom( playlistWindow->geometry().bottom() );
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
        goto MOVE_setGeometry;


    // ============================== Snapping to lyrics window ==============================

    if( I_window != lyricsWindow && lyricsWindow->isVisible() )
    {
        // If it is main window and playlist window has already been snapped to it, jump.
        if( I_window == mainWindow && breadthFirstSearch( TP::lyricsWindow ) )
            goto MOVE_setGeometry;

        // ------------------ Step 1 (edge sticks to edge) ------------------

        SnapType snapType { checkSnapType( I_geometry, lyricsWindow->geometry() ) };

        switch( snapType )
        {
        case SnapType::ToBottom :
            I_geometry.moveTop( lyricsWindow->geometry().bottom() + 1 );
            isVerticallySnapped = true;
            break;

        case SnapType::ToTop :
            I_geometry.moveBottom( lyricsWindow->geometry().top() - 1 );
            isVerticallySnapped = true;
            break;

        case SnapType::ToRight :
            I_geometry.moveLeft( lyricsWindow->geometry().right() + 1 );
            isHorizontallySnapped = true;
            break;

        case SnapType::ToLeft :
            I_geometry.moveRight( lyricsWindow->geometry().left() - 1 );
            isHorizontallySnapped = true;
            break;

        case SnapType::NonSnap :
            goto MOVE_setGeometry;
        }

        // ---------- Step 2 (edge alignment in the vertical direction) ----------

        switch( snapType )
        {
        case SnapType::ToTop :
        case SnapType::ToBottom :

            // Left edge alignment
            if( std::abs( I_geometry.left() - lyricsWindow->geometry().left() )
                    <= TP::snapRange
                && ! isHorizontallySnapped )
            {
                I_geometry.moveLeft( lyricsWindow->geometry().left() );
                break;
            }

            // Right edge alignment
            if( std::abs( I_geometry.right() - lyricsWindow->geometry().right() )
                    <= TP::snapRange
                && ! isHorizontallySnapped )
            {
                I_geometry.moveRight( lyricsWindow->geometry().right() );
                break;
            }

            break;

        case SnapType::ToLeft :
        case SnapType::ToRight :

            // Top edge alignment
            if( std::abs( I_geometry.top() - lyricsWindow->geometry().top() )
                    <= TP::snapRange
                && ! isVerticallySnapped )
            {
                I_geometry.moveTop( lyricsWindow->geometry().top() );
                break;
            }

            // Bottom edge alignment
            if( std::abs( I_geometry.bottom() - lyricsWindow->geometry().bottom() )
                    <= TP::snapRange
                && ! isVerticallySnapped )
            {
                I_geometry.moveBottom( lyricsWindow->geometry().bottom() );
                break;
            }

            break;

        default:
            break;
        }       // switch( snapType )
    }       // if( window != lyricsWindow && lyricsWindow->isVisible() )


// ==================== Move window ====================
MOVE_setGeometry:

    I_window->setGeometry( I_geometry );

    // If window is main window, move the snapped window(s) as well
    if( I_window == mainWindow )
    {
        unsnapInvisibleWindows();

        // Move playlist window
        if ( breadthFirstSearch( TP::playlistWindow ) )
            playlistWindow->move( I_geometry.topLeft() + snapPosition_playlistWindow );

        // Move lyrics window
        if ( breadthFirstSearch( TP::lyricsWindow ) )
            lyricsWindow->move( I_geometry.topLeft() + snapPosition_lyricsWindow );
    }
}


void
TP_MainClass::slot_resizeWindow( QWidget *I_window, QRect I_geometry, TP::CursorPositionType I_resizeType )
{
    // While resizing, a window can only be snapped (or aligned) once.

    if ( I_window != mainWindow )
    {
        // ------------------ Step 1 (edge sticks to edge) ------------------

        switch( checkSnapType( I_geometry, mainWindow->geometry() ) )
        {
        case SnapType::ToTop :
            if( I_resizeType == TP::CursorPositionType::Bottom )
            {
                I_geometry.setBottom( mainWindow->geometry().top() - 1 );
                goto RESIZE_setGeometry;
            }
            break;

        case SnapType::ToRight :
            if( I_resizeType == TP::CursorPositionType::Left )
            {
                I_geometry.setLeft( mainWindow->geometry().right() + 1 );
                goto RESIZE_setGeometry;
            }
            break;

        case SnapType::ToLeft :
            if( I_resizeType == TP::CursorPositionType::Right )
            {
                I_geometry.setRight( mainWindow->geometry().left() - 1 );
                goto RESIZE_setGeometry;
            }
            break;

        default :
            break;
        }       // switch( snapType )

        // ---------- Step 2 (edge alignment in the vertical direction) ----------

        switch( checkAdjacentType( I_geometry, mainWindow->geometry() ) )
        {
        case SnapType::ToTop :
        case SnapType::ToBottom :

            // Left edge alignment
            if( std::abs( I_geometry.left() - mainWindow->geometry().left() )
                    <= TP::snapRange
                    && I_resizeType == TP::CursorPositionType::Left )
            {
                I_geometry.setLeft( mainWindow->geometry().left() );
                goto RESIZE_setGeometry;
            }

            // Right edge alignment
            if( std::abs( I_geometry.right() - mainWindow->geometry().right() )
                    <= TP::snapRange
                && I_resizeType == TP::CursorPositionType::Right )
            {
                I_geometry.setRight( mainWindow->geometry().right() );
                goto RESIZE_setGeometry;
            }

            break;

        case SnapType::ToLeft :
        case SnapType::ToRight :

            // Bottom edge alignment
            if( std::abs( I_geometry.bottom() - mainWindow->geometry().bottom() )
                    <= TP::snapRange
                && I_resizeType == TP::CursorPositionType::Bottom )
            {
                I_geometry.setBottom( mainWindow->geometry().bottom() );
                goto RESIZE_setGeometry;
            }

            break;

        default:
            break;
        }       // switch( snapType )
    }       //if ( window != mainWindow )

    // ============================== Snapping to playlist window ==============================

    if( I_window != playlistWindow && playlistWindow->isVisible() )
    {
        // ------------------ Step 1 (edge sticks to edge) ------------------

        switch( checkSnapType( I_geometry, playlistWindow->geometry() ) )
        {
        case SnapType::ToTop :
            if( I_resizeType == TP::CursorPositionType::Bottom )
            {
                I_geometry.setBottom( playlistWindow->geometry().top() - 1 );
                goto RESIZE_setGeometry;
            }
            break;

        case SnapType::ToRight :
            if( I_resizeType == TP::CursorPositionType::Left )
            {
                I_geometry.setLeft( playlistWindow->geometry().right() + 1 );
                goto RESIZE_setGeometry;
            }
            break;

        case SnapType::ToLeft :
            if( I_resizeType == TP::CursorPositionType::Right )
            {
                I_geometry.setRight( playlistWindow->geometry().left() - 1 );
                goto RESIZE_setGeometry;
            }
            break;

        default :
            break;
        }       // switch( snapType )

        // ---------- Step 2 (edge alignment in the vertical direction) ----------

        switch( checkAdjacentType( I_geometry, playlistWindow->geometry() ) )
        {
        case SnapType::ToTop :
        case SnapType::ToBottom :

            // Left edge alignment
            if( std::abs( I_geometry.left() - playlistWindow->geometry().left() )
                    <= TP::snapRange
                    && I_resizeType == TP::CursorPositionType::Left )
            {
                I_geometry.setLeft( playlistWindow->geometry().left() );
                goto RESIZE_setGeometry;
            }

            // Right edge alignment
            if( std::abs( I_geometry.right() - playlistWindow->geometry().right() )
                    <= TP::snapRange
                && I_resizeType == TP::CursorPositionType::Right )
            {
                I_geometry.setRight( playlistWindow->geometry().right() );
                goto RESIZE_setGeometry;
            }

            break;

        case SnapType::ToLeft :
        case SnapType::ToRight :

            // Bottom edge alignment
            if( std::abs( I_geometry.bottom() - playlistWindow->geometry().bottom() )
                    <= TP::snapRange
                && I_resizeType == TP::CursorPositionType::Bottom )
            {
                I_geometry.setBottom( playlistWindow->geometry().bottom() );
                goto RESIZE_setGeometry;
            }

            break;

        default:
            break;
        }       // switch( snapType )
    }       // if( window != playlistWindow && playlistWindow->isVisible() )


    // ============================== Snapping to lyrics window ==============================

    if( I_window != lyricsWindow && lyricsWindow->isVisible() )
    {
        // ------------------ Step 1 (edge sticks to edge) ------------------

        switch( checkSnapType( I_geometry, lyricsWindow->geometry() ) )
        {
        case SnapType::ToTop :
            if( I_resizeType == TP::CursorPositionType::Bottom )
            {
                I_geometry.setBottom( lyricsWindow->geometry().top() - 1 );
                goto RESIZE_setGeometry;
            }
            break;

        case SnapType::ToRight :
            if( I_resizeType == TP::CursorPositionType::Left )
            {
                I_geometry.setLeft( lyricsWindow->geometry().right() + 1 );
                goto RESIZE_setGeometry;
            }
            break;

        case SnapType::ToLeft :
            if( I_resizeType == TP::CursorPositionType::Right )
            {
                I_geometry.setRight( lyricsWindow->geometry().left() - 1 );
                goto RESIZE_setGeometry;
            }
            break;

        default :
            break;
        }       // switch( snapType )

        // ---------- Step 2 (edge alignment in the vertical direction) ----------

        switch( checkAdjacentType( I_geometry, lyricsWindow->geometry() ) )
        {
        case SnapType::ToTop :
        case SnapType::ToBottom :

            // Left edge alignment
            if( std::abs( I_geometry.left() - lyricsWindow->geometry().left() )
                    <= TP::snapRange
                    && I_resizeType == TP::CursorPositionType::Left )
            {
                I_geometry.setLeft( lyricsWindow->geometry().left() );
                goto RESIZE_setGeometry;
            }

            // Right edge alignment
            if( std::abs( I_geometry.right() - lyricsWindow->geometry().right() )
                    <= TP::snapRange
                && I_resizeType == TP::CursorPositionType::Right )
            {
                I_geometry.setRight( lyricsWindow->geometry().right() );
                goto RESIZE_setGeometry;
            }

            break;

        case SnapType::ToLeft :
        case SnapType::ToRight :

            // Bottom edge alignment
            if( std::abs( I_geometry.bottom() - lyricsWindow->geometry().bottom() )
                    <= TP::snapRange
                && I_resizeType == TP::CursorPositionType::Bottom )
            {
                I_geometry.setBottom( lyricsWindow->geometry().bottom() );
                goto RESIZE_setGeometry;
            }

            break;

        default:
            break;
        }       // switch( snapType )
    }       // if( window != lyricsWindow && lyricsWindow->isVisible() )


RESIZE_setGeometry:
    I_window->setGeometry( I_geometry );
}


void
TP_MainClass::slot_refreshSnapStatus()
{
    // main window <---> playlist window
    if( playlistWindow->isVisible()
            && checkAdjacentType( mainWindow->geometry(), playlistWindow->geometry() ) != SnapType::NonSnap )
    {
        snapStatus [ TP::mainWindow ][ TP::playlistWindow ] = true;
        snapStatus [ TP::playlistWindow ][ TP::mainWindow ] = true;
    }
    else
    {
        snapStatus [ TP::mainWindow ][ TP::playlistWindow ] = false;
        snapStatus [ TP::playlistWindow ][ TP::mainWindow ] = false;
    }

    // main window <---> lyrics window
    if( lyricsWindow->isVisible()
            && checkAdjacentType( mainWindow->geometry(), lyricsWindow->geometry() ) != SnapType::NonSnap )
    {
        snapStatus [ TP::mainWindow ][ TP::lyricsWindow ] = true;
        snapStatus [ TP::lyricsWindow ][ TP::mainWindow ] = true;
    }
    else
    {
        snapStatus [ TP::mainWindow ][ TP::lyricsWindow ] = false;
        snapStatus [ TP::lyricsWindow ][ TP::mainWindow ] = false;
    }

    // playlist window <---> lyrics window
    if( playlistWindow->isVisible() && lyricsWindow->isVisible()
            && checkAdjacentType( playlistWindow->geometry(), lyricsWindow->geometry() ) != SnapType::NonSnap )
    {
        snapStatus [ TP::playlistWindow ][ TP::lyricsWindow ] = true;
        snapStatus [ TP::lyricsWindow ][ TP::playlistWindow ] = true;
    }
    else
    {
        snapStatus [ TP::playlistWindow ][ TP::lyricsWindow ] = false;
        snapStatus [ TP::lyricsWindow ][ TP::playlistWindow ] = false;
    }

    // QWidget::pos() cannot be used here.
    // pos() is not always identical to geometry().topLeft().
    if ( breadthFirstSearch( TP::playlistWindow ) )
        snapPosition_playlistWindow = playlistWindow->geometry().topLeft() - mainWindow->geometry().topLeft();
    if ( breadthFirstSearch( TP::lyricsWindow ) )
        snapPosition_lyricsWindow = lyricsWindow->geometry().topLeft() - mainWindow->geometry().topLeft();
}


void
TP_MainClass::slot_itemDoubleClicked( QListWidgetItem *I_item )
{
    // Double clicking list item interrupts the playing order.
    // Therefore, previous and next items need to be initialized.
    playlistWindow->slot_clearPreviousAndNext();
    playItem( I_item );
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
        qDebug() << "[Main Class] state status inconsistent! Program will fix it manually.";
        mainWindow->setPlay();
        break;
    }
}


void
TP_MainClass::slot_nextButtonPushed()
{
    if( TP::config().getPlayMode() == TP::PlayMode::Shuffle )
        playItem( playlistWindow->getNextItem_shuffle() );
    else
        playItem( playlistWindow->getNextItem() );
}


void
TP_MainClass::slot_previousButtonPushed()
{
    if( TP::config().getPlayMode() == TP::PlayMode::Shuffle )
        playItem( playlistWindow->getPreviousItem_shuffle() );
    else
        playItem( playlistWindow->getPreviousItem() );
}


void
TP_MainClass::slot_interruptingStopTriggered()
{
    if( mediaPlayer->playbackState() != QMediaPlayer::StoppedState )
    {
        b_isStopInterrupting = true;
        mediaPlayer->stop();
    }
}

void
TP_MainClass::slot_playbackStateChanged( QMediaPlayer::PlaybackState I_state )
{
    switch ( TP::playbackState() = I_state )
    {
    case QMediaPlayer::PlayingState:
        qDebug() << "[Media Player] Playback state changed to PlayingState.";
        slot_setVolume( linearVolume );
        mainWindow->setPlay();
        playlistWindow->setCurrentItemBold();
        break;

    case QMediaPlayer::PausedState:
        qDebug() << "[Media Player] Playback state changed to PauseState.";
        mainWindow->setPause();

        break;

    case QMediaPlayer::StoppedState:
        qDebug() << "[Media Player] Playback state changed to StoppedState.";
        TP::PlayMode mode { TP::config().getPlayMode() };

        if( ! b_isStopInterrupting && mode != TP::PlayMode::SingleTime
                && mediaPlayer->error() == QMediaPlayer::NoError )
            switch( mode )
            {
            case TP::PlayMode::Repeat :
                mediaPlayer->play();
                break;

            case TP::PlayMode::Sequential :
            case TP::PlayMode::Shuffle :
                slot_nextButtonPushed();
                break;

            default:
                break;
            }
        else
        {
            mainWindow->setStop();
            mainWindow->setCover( {} );

            playlistWindow->unsetCurrentItemBold();

            lyricsWindow->clearLyricsViewer();

            if( mediaPlayer->error() != QMediaPlayer::NoError )
                mainWindow->setAudioInfoLabelTexts(
                            { tr( "QMediaPlayer error: " ) + mediaPlayer->errorString() }
                            );

            mediaPlayer->setSource( {} );               // Cease I/O operations of current file
        }

        b_isStopInterrupting = false;
        break;
    }
}


void
TP_MainClass::slot_mediaStatusChanged ( QMediaPlayer::MediaStatus I_status )
{
    switch ( I_status )
    {
    case QMediaPlayer::NoMedia :
        qDebug( "[Media Player] Media status changed to NoMedia." );
        break;

    case QMediaPlayer::LoadingMedia :
        qDebug( "[Media Player] Media status changed to LoadingMedia." );
        break;

    case QMediaPlayer::LoadedMedia :
        qDebug( "[Media Player] Media status changed to LoadedMedia." );
        break;

    case QMediaPlayer::StalledMedia :
        qDebug( "[Media Player] Media status changed to StalledMedia." );
        break;

    case QMediaPlayer::BufferingMedia :
        qDebug( "[Media Player] Media status changed to BufferingMedia." );
        break;

    case QMediaPlayer::BufferedMedia :
        qDebug( "[Media Player] Media status changed to BufferedMedia." );
        break;

    case QMediaPlayer::EndOfMedia :
        qDebug( "[Media Player] Media status changed to EndOfMedia." );
        break;

    case QMediaPlayer::InvalidMedia :
        qDebug( "[Media Player] Media status changed to InvalidMedia." );
        break;
    }
}


void
TP_MainClass::slot_deviceChanged() const
{
    qDebug()<< "[Audio Output] Current audio output device is "<< mediaPlayer->audioOutput()->device().description();
}

/*
void
TP_MainClass::slot_mediaPlayerError( QMediaPlayer::Error , const QString &I_qstr_error ) const
{
    QMessageBox::critical(
                nullptr,                                // QWidget *parent
                tr( "Media Player Error" ),             // const QString &title
                // const QString &text
                tr( "An error of QMediaPlayer has occurred:\n" ) +
                I_qstr_error );
}
*/

void
TP_MainClass::slot_positionChanged( qint64 I_ms )
{
    mainWindow  ->updatePosition( I_ms );
    lyricsWindow->updatePosition( I_ms );
}


void
TP_MainClass::slot_setVolume( float I_linearVolume )
{
    linearVolume = I_linearVolume;

    if( mediaPlayer->playbackState() != QMediaPlayer::PlayingState )
        return;

    auto multiplier {
        std::pow( 10, TP::getReplayGainFromItem( TP::currentItem() ) / 20.0 )
    };                                              // 10^(Gain/20)
    audioOutput->setVolume( linearVolume * multiplier );
}

// *****************************************************************
// private
// *****************************************************************

void
TP_MainClass::initializeConnection()
{
    // Playerback control
    connect( mediaPlayer,   &QMediaPlayer::positionChanged,
             this,          &TP_MainClass::slot_positionChanged );
    connect( mediaPlayer,   &QMediaPlayer::playbackStateChanged,
             this,          &TP_MainClass::slot_playbackStateChanged );
    connect( mediaPlayer,   &QMediaPlayer::mediaStatusChanged,
             this,          &TP_MainClass::slot_mediaStatusChanged );
    /*connect( mediaPlayer,   &QMediaPlayer::errorOccurred,
             this,          &TP_MainClass::slot_mediaPlayerError );*/
    connect( audioOutput,   &QAudioOutput::deviceChanged,
             this,          &TP_MainClass::slot_deviceChanged );

    connect( mainWindow,    &TP_MainWindow::signal_playButtonPushed,
             this,          &TP_MainClass::slot_playButtonPushed );
    connect( mainWindow,    &TP_MainWindow::signal_pauseButtonPushed,
             mediaPlayer,   &QMediaPlayer::pause );
    connect( mainWindow,    &TP_MainWindow::signal_stopButtonPushed,
             this,          &TP_MainClass::slot_interruptingStopTriggered );

    connect( mainWindow,    &TP_MainWindow::signal_timeSliderPressed,
             mediaPlayer,   &QMediaPlayer::setPosition );
    connect( lyricsWindow,  &TP_LyricsWindow::signal_lyricsDoubleClicked,
             mediaPlayer,   &QMediaPlayer::setPosition );

    // Source switching
    connect( mainWindow,        &TP_MainWindow::signal_modeIsNotShuffle,
             playlistWindow,    &TP_PlaylistWindow::slot_clearPreviousAndNext );
    connect( mainWindow,        &TP_MainWindow::signal_nextButtonPushed,
             this,              &TP_MainClass::slot_nextButtonPushed );
    connect( mainWindow,        &TP_MainWindow::signal_previousButtonPushed,
             this,              &TP_MainClass::slot_previousButtonPushed );

    // Volume control
    connect( mainWindow,    &TP_MainWindow::signal_volumeSliderValueChanged,
             this,          &TP_MainClass::slot_setVolume );

    // Program exiting, Windows minimizing & restoring
    connect( mainWindow,        &TP_MainWindow::signal_exitProgram,
             this,              &TP_MainClass::slot_exitProgram );
    connect( mainWindow,        &TP_MainWindow::signal_minimizeWindow,
             this,              &TP_MainClass::slot_minimizeWindow );
    connect( mainWindow,        &TP_MainWindow::signal_restoreWindow,
             this,              &TP_MainClass::slot_restoreWindow );
    connect( mainWindow,        &TP_MainWindow::signal_activateWindow,
             playlistWindow,    &TP_PlaylistWindow::slot_activateWindow );
    connect( mainWindow,        &TP_MainWindow::signal_activateWindow,
             lyricsWindow,      &TP_LyricsWindow::slot_activateWindow );

    // Windows moving & resizing
    connect( mainWindow,    &TP_MainWindow::signal_moveWindow,
             this,          &TP_MainClass::slot_moveWindow );
    connect( mainWindow,    &TP_MainWindow::signal_resizeWindow,
             this,          &TP_MainClass::slot_resizeWindow );
    connect( mainWindow,    &TP_MainWindow::signal_windowChanged,
             this,          &TP_MainClass::slot_refreshSnapStatus );

    connect( playlistWindow,    &TP_PlaylistWindow::signal_moveWindow,
             this,              &TP_MainClass::slot_moveWindow );
    connect( playlistWindow,    &TP_PlaylistWindow::signal_resizeWindow,
             this,              &TP_MainClass::slot_resizeWindow );
    connect( playlistWindow,    &TP_PlaylistWindow::signal_windowChanged,
             this,              &TP_MainClass::slot_refreshSnapStatus );

    connect( lyricsWindow,  &TP_LyricsWindow::signal_moveWindow,
             this,          &TP_MainClass::slot_moveWindow );
    connect( lyricsWindow,  &TP_LyricsWindow::signal_resizeWindow,
             this,          &TP_MainClass::slot_resizeWindow );
    connect( lyricsWindow,  &TP_LyricsWindow::signal_windowChanged,
             this,          &TP_MainClass::slot_refreshSnapStatus );

    // Showing and hiding PlaylistWindow
    connect( playlistWindow,    &TP_PlaylistWindow::signal_hidden,
             mainWindow,        &TP_MainWindow::slot_playlistWindowHidden );
    connect( playlistWindow,    &TP_PlaylistWindow::signal_shown,
             mainWindow,        &TP_MainWindow::slot_playlistWindowShown );
    connect( mainWindow,        &TP_MainWindow::signal_openPlaylistWindow,
             playlistWindow,    &TP_PlaylistWindow::show );
    connect( mainWindow,        &TP_MainWindow::signal_hidePlaylistWindow,
             playlistWindow,    &TP_PlaylistWindow::hide );

    // Showing and hiding LyricsWindow
    connect( lyricsWindow,  &TP_LyricsWindow::signal_hidden,
             mainWindow,    &TP_MainWindow::slot_lyricsWindowHidden );
    connect( lyricsWindow,  &TP_LyricsWindow::signal_shown,
             mainWindow,    &TP_MainWindow::slot_lyricsWindowShown );
    connect( mainWindow,    &TP_MainWindow::signal_openLyricsWindow,
             lyricsWindow,  &TP_LyricsWindow::show );
    connect( mainWindow,    &TP_MainWindow::signal_hideLyricsWindow,
             lyricsWindow,  &TP_LyricsWindow::hide );

    // ConfigWindow related
    connect( mainWindow,        &TP_MainWindow::signal_openConfigWindow,
             configWindow,      &TP_ConfigWindow::exec );
    connect( configWindow,      &TP_ConfigWindow::signal_audioDeviceChanged,
             audioOutput,       &QAudioOutput::setDevice );
    connect( configWindow,      &TP_ConfigWindow::signal_audioInfoLabelFontChanged,
             mainWindow,        &TP_MainWindow::slot_changeFontOfAudioInfoLabel );
    connect( configWindow,      &TP_ConfigWindow::signal_playlistFontChanged,
             playlistWindow,    &TP_PlaylistWindow::slot_refreshFont );
    connect( configWindow,      &TP_ConfigWindow::signal_lyricsFontChanged,
             lyricsWindow,      &TP_LyricsWindow::slot_refreshFont );

    // Signals from FileListWidgets through PlaylistWindow
    connect( playlistWindow,    &TP_PlaylistWindow::signal_currentItemRemoved,
             this,              &TP_MainClass::slot_interruptingStopTriggered );
    connect( playlistWindow,    &TP_PlaylistWindow::signal_itemDoubleClicked,
             this,              &TP_MainClass::slot_itemDoubleClicked );
}


void
TP_MainClass::unsnapInvisibleWindows()
{
    if( ! playlistWindow->isVisible() )
        for( int i {}; i < TP::numberOfWindows; i++ )
        {
            snapStatus [ i ][ TP::playlistWindow ] = false;
            snapStatus [ TP::playlistWindow ][ i ] = false;
        }

    if( ! lyricsWindow->isVisible() )
        for( int i {}; i < TP::numberOfWindows; i++ )
        {
            snapStatus [ i ][ TP::lyricsWindow ] = false;
            snapStatus [ TP::lyricsWindow ][ i ] = false;
        }
}


TP_MainClass::SnapType
TP_MainClass::checkSnapType( const QRect &I_geometry1, const QRect &I_geometry2 ) const
{
    // Intersect horizontally
    if( I_geometry1.right() >= I_geometry2.left()
            && I_geometry1.left() <= I_geometry2.right() )
    {
        if( std::abs( I_geometry1.top() - I_geometry2.bottom() ) <= TP::snapRange )
            return SnapType::ToBottom;

        if( std::abs( I_geometry1.bottom() - I_geometry2.top() ) <= TP::snapRange )
            return SnapType::ToTop;
    }

    // Intersect vertically
    if( I_geometry1.bottom() >= I_geometry2.top()
            && I_geometry1.top() <= I_geometry2.bottom() )
    {
        if( std::abs( I_geometry1.right() - I_geometry2.left() ) <= TP::snapRange )
            return SnapType::ToLeft;

        if( std::abs( I_geometry1.left() - I_geometry2.right() ) <= TP::snapRange )
            return SnapType::ToRight;
    }

    return SnapType::NonSnap ;
}


TP_MainClass::SnapType
TP_MainClass::checkAdjacentType( const QRect &I_geometry1, const QRect &I_geometry2 ) const
{
    // Intersect horizontally
    if( I_geometry1.right() >= I_geometry2.left()
            && I_geometry1.left() <= I_geometry2.right() )
    {
        if( I_geometry1.top() - I_geometry2.bottom() == 1 )
            return SnapType::ToBottom;

        if( I_geometry2.top() - I_geometry1.bottom() == 1 )
            return SnapType::ToTop;
    }

    // Intersect vertically
    if( I_geometry1.bottom() >= I_geometry2.top()
            && I_geometry1.top() <= I_geometry2.bottom() )
    {
        if( I_geometry2.left() - I_geometry1.right() == 1 )
            return SnapType::ToLeft;

        if( I_geometry1.left() - I_geometry2.right() == 1 )
            return SnapType::ToRight;
    }

    return SnapType::NonSnap;
}


bool
TP_MainClass::breadthFirstSearch( int I_idx ) const
{
    int idx_Current {};
    bool isVisited [ TP::numberOfWindows ] {};
    std::queue < int > queue {};

    isVisited [ TP::mainWindow ] = true;
    // Search always start from main window.
    queue.push( TP::mainWindow );

    while( ! queue.empty() )
    {
        idx_Current = queue.front();
        queue.pop();

        for( int i { 1 }; i < TP::numberOfWindows; i++ )
            if( ! isVisited[ i ] && snapStatus[ idx_Current ][ i ] )
            {
                if( i == I_idx )
                    return true;
                else
                {
                    isVisited[ i ] = true;
                    queue.push( i );
                }
             }
    }

    return false;
}


void
TP_MainClass::playItem( QListWidgetItem *I_item )
{
    if( I_item == nullptr )
        return;

    switch ( I_item->data( TP::role_SourceType ).value< TP::SourceType >() )
    {
    case TP::SourceType::SingleFile :
        playFile( I_item );
        break;
    default:
        break;
    }
}


void
TP_MainClass::playFile( QListWidgetItem *I_item )
{
    const auto &url { I_item->data( TP::role_URL ).toUrl() };
    qDebug() << "[Main Class] playFile() is trying to play local file URL: " << url;
    const auto &qstr_localFilePath { url.toLocalFile() };

    if( std::filesystem::exists( qstr_localFilePath.
#ifdef Q_OS_WIN
                                 toStdWString()
#else
                                 toLocal8Bit().constData()
#endif
                                 ) )
    {
        slot_interruptingStopTriggered();

        TP::storeInformation( I_item );                                 // Refresh audio info
        mainWindow->setAudioInfomation( I_item );

        // ------------------------------ Read cover art ------------------------------
        QImage coverArt {};
        TagLib::FileRef *fileRef {
            new TagLib::FileRef { qstr_localFilePath
#ifdef Q_OS_WIN
                        .toStdWString().c_str()
#else
                        .toLocal8Bit().constData()
#endif
                                } };

        switch( I_item->data( TP::role_AudioFormat ).value< TP::AudioFormat >() )
        {
        case TP::AudioFormat::FLAC :
        {
            // FLAC files may store cover in the file itself,
            // or as a part of Xiph comment or ID3v2 frame.
            TagLib::FLAC::File *flacFile {
                dynamic_cast< TagLib::FLAC::File * >( fileRef->file() )
            };

            coverArt = getCoverImage( flacFile );

            if( flacFile->hasXiphComment() && coverArt.isNull() )
                coverArt = getCoverImage( flacFile->xiphComment() );

            if( flacFile->hasID3v2Tag() && coverArt.isNull() )
                coverArt = getCoverImage( flacFile->ID3v2Tag() );

            break;
        }

        case TP::AudioFormat::ALAC :
        case TP::AudioFormat::AAC :
        {
            // MPEG-4 audio files may store cover as MP4 tag.
            TagLib::MP4::File *mp4File {
                dynamic_cast< TagLib::MP4::File * >( fileRef->file() )
            };

            if( mp4File->hasMP4Tag() )
                coverArt = getCoverImage( mp4File->tag() );

            break;
        }

        case TP::AudioFormat::MP3 :
        {
            // MP3 audio files may store cover as ID3v2 frame.
            TagLib::MPEG::File *mp3File {
                dynamic_cast< TagLib::MPEG::File * >( fileRef->file() )
            };

            if( mp3File->hasID3v2Tag() )
                coverArt = getCoverImage( mp3File->ID3v2Tag() );

            break;
        }

        case TP::AudioFormat::OGG :
        {
            // Vorbis audio files may store cover as Xiph comment.
            TagLib::Ogg::Vorbis::File *vorbisFile {
                dynamic_cast< TagLib::Ogg::Vorbis::File * >( fileRef->file() )
            };
            coverArt = getCoverImage( vorbisFile->tag() );

            break;
        }

        default:
            break;
        }

        delete fileRef;

        mainWindow->setCover( coverArt );

        playlistWindow->unsetCurrentItemBold();
        TP::currentItem() = I_item;
        playlistWindow->setCurrentItem( I_item );
        playlistWindow->refreshItemShowingTitle( I_item );

        lyricsWindow->readLyricsFileFromCurrentItem();

        mediaPlayer->setSource( url );
#ifdef Q_OS_WIN
        QTimer::singleShot( 500, mediaPlayer, &QMediaPlayer::play );
#else
        mediaPlayer->play();
#endif
    }       // if( std::filesystem::exists
    else
    {
        slot_interruptingStopTriggered();
        mainWindow->setAudioInfoLabelTexts( { tr( "File Not Exists" ) } );
    }
}


QImage
TP_MainClass::getCoverImage( TagLib::FLAC::File *I_flacFile )
{
    const auto &pictureList { I_flacFile->pictureList() };
    if( pictureList.size() > 0 )
        return QImage::fromData(
                    QByteArray { pictureList[0]->data().data(), pictureList[0]->data().size() }
                    );

    return {};
}


QImage
TP_MainClass::getCoverImage( TagLib::Ogg::XiphComment *I_xiphComment )
{
    const auto &pictureList { I_xiphComment->pictureList() };
    if( pictureList.size() > 0 )
        return QImage::fromData(
                    QByteArray { pictureList[0]->data().data(), pictureList[0]->data().size() }
                    );

    return {};
}


QImage
TP_MainClass::getCoverImage( TagLib::ID3v2::Tag *I_id3v2Tag )
{
    const auto &frameList { I_id3v2Tag->frameList( "APIC" ) };
    if( ! frameList.isEmpty() )
    {
        auto *pictureFrame { static_cast< TagLib::ID3v2::AttachedPictureFrame * >( frameList.front() ) };
        return QImage::fromData(
                QByteArray { pictureFrame->picture().data(), pictureFrame->picture().size() }
                );
    }

    return {};
}


QImage
TP_MainClass::getCoverImage( TagLib::MP4::Tag *I_mp4Tag )
{
    if( I_mp4Tag->contains( "covr" ) )
    {
        const auto &coverArtList { I_mp4Tag->itemMap()[ "covr" ].toCoverArtList() };
        if( !coverArtList.isEmpty() )
        {
            const auto &coverArt { coverArtList.front() };
            return QImage::fromData(
                        QByteArray { coverArt.data().data(), coverArt.data().size() }
                        );
        }
    }

    return {};
}
