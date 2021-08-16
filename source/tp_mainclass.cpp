#include "tp_mainclass.h"

#include "tp_globalconst.h"

#include "tp_mainwindow.h"

#include "tp_playlistwindow.h"
#include "tp_filelistwidget.h"

#include <QAbstractButton>
#include <QApplication>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>
#include <QWindow>

TP_MainClass::TP_MainClass() :
    QObject { nullptr }
  , mainWindow { new TP_MainWindow {} }
  , playlistWindow { new TP_PlaylistWindow {} }
  , audioOutput { new QAudioOutput { this } }
  , mediaPlayer { new QMediaPlayer { this } }
{
    mediaPlayer->setAudioOutput(audioOutput);
    audioOutput->setDevice(QAudioDevice());
    audioOutput->setVolume(50);

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
    connect(I_FilelistWidget,   &TP_FileListWidget::itemDoubleClicked,
            this,               &TP_MainClass::slot_playFile);
}

void
TP_MainClass::slot_playFile(QListWidgetItem *I_listWidgetItem)
{
    QString qstr_FilePath { I_listWidgetItem->data(TP::role_Path).value<QString>() };
    qDebug() << "[SLOT] TP_MainClass::slot_playFile: " << qstr_FilePath;
    QUrl fileURL { qstr_FilePath };

    mediaPlayer->setSource ( fileURL );
    mediaPlayer->play();
}

// *****************************************************************
// private
// *****************************************************************

void
TP_MainClass::initializeConnection()
{
    // Showing and hiding PlaylistWindow
    connect(playlistWindow, &TP_PlaylistWindow::signal_Hidden,
            mainWindow,     &TP_MainWindow::slot_PlaylistWindow_Hidden);
    connect(playlistWindow, &TP_PlaylistWindow::signal_Shown,
            mainWindow,     &TP_MainWindow::slot_PlaylistWindow_Shown);
    connect(mainWindow,     &TP_MainWindow::signal_openPlaylistWindow,
            playlistWindow, &TP_PlaylistWindow::show);
    connect(mainWindow,     &TP_MainWindow::signal_hidePlaylistWindow,
            playlistWindow, &TP_PlaylistWindow::hide);

    // Make PlaylistWindow be able to emit signal for connecting its FileListWidget
    connect(playlistWindow, &TP_PlaylistWindow::signal_NewFilelistWidgetCreated,
            this,           &TP_MainClass::slot_connectFilelistWidget);

    // Double click
}
