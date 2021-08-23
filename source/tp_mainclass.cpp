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
#include <QMediaPlayer>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>
#include <QWindow>

#include <filesystem>

// Headers of TagLib
#include <fileref.h>
#include <tag.h>

#include <flacproperties.h>

TP_MainClass::TP_MainClass() :
    QObject { nullptr }
  , mainWindow { new TP_MainWindow {} }
  , playlistWindow { new TP_PlaylistWindow {} }
  , audioOutput { new QAudioOutput { this } }
  , mediaPlayer { new QMediaPlayer { this } }
{
    mediaPlayer->setAudioOutput(audioOutput);
    qDebug()<< "Current audio output device is "<< mediaPlayer->audioOutput()->device().description();

    qreal linearVolume =  QAudio::convertVolume(0.5,
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
TP_MainClass::slot_connectFilelistWidget(TP_FileListWidget *I_FilelistWidget)
{
    qDebug() << "[SLOT] slot_connectFilelistWidget -- list's name is " << I_FilelistWidget->getListName();
    connect(I_FilelistWidget,   &TP_FileListWidget::itemDoubleClicked,
            this,               &TP_MainClass::slot_playItem);
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
// *****************************************************************
// private slots:
// *****************************************************************


// *****************************************************************
// private
// *****************************************************************

void
TP_MainClass::initializeConnection()
{
    // Showing and hiding PlaylistWindow
    connect(playlistWindow, &TP_PlaylistWindow::signal_Hidden,
            mainWindow,     &TP_MainWindow::slot_PlaylistWindowHidden);
    connect(playlistWindow, &TP_PlaylistWindow::signal_Shown,
            mainWindow,     &TP_MainWindow::slot_PlaylistWindowShown);
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
    {
        TagLib::FileRef fileRef { url.toLocalFile().toLocal8Bit().constData() };

        // Set audio property labels
        QString qstr_Format;
        int bitRate = fileRef.audioProperties()->bitrate();
        int sampleRate = fileRef.audioProperties()->sampleRate() / 1000;
        int bitDepth = -1;
        int duration = fileRef.audioProperties()->lengthInSeconds();
        if( duration != I_listWidgetItem->data( TP::role_Duration ).value<int>() )
            I_listWidgetItem->setData( TP::role_Duration, duration );

        TP::FileFormat format { I_listWidgetItem->data(TP::role_FileType).value<TP::FileFormat>() };
        switch (format)
        {
        case TP::FileFormat::FLAC :
            qstr_Format = QString( "FLAC" );
            bitDepth = dynamic_cast<TagLib::FLAC::Properties *>( fileRef.audioProperties() )->bitsPerSample();
            break;
        case TP::FileFormat::MP3 :
            qstr_Format = QString( "MP3" );
            break;
        }

        mainWindow->setCurrentAudioProperties( qstr_Format, bitDepth, sampleRate, bitRate, duration );
    }
    else
        return;

    mediaPlayer->setSource( url );
    mediaPlayer->play();
}
