#include "tp_mainclass.h"

#include "tp_mainwindow.h"
#include "tp_playlistwindow.h"

#include <QGuiApplication>
#include <QScreen>
#include <QWindow>

TP_MainClass::TP_MainClass() :
    QObject { nullptr }
  , tp_MainWindow { new TP_MainWindow {} }
  , tp_PlaylistWindow { new TP_PlaylistWindow {} }
{
    tp_MainWindow->show();
}

TP_MainClass::~TP_MainClass()
{
    delete tp_MainWindow;
    delete tp_PlaylistWindow;
}

void
TP_MainClass::initializePosition()
{
    QPoint globalPositionBottomLeft_PlaylistWindow = tp_MainWindow->geometry().bottomLeft();
    qDebug()<<"globalPositionBottomLeft_PlaylistWindow: "<<globalPositionBottomLeft_PlaylistWindow;
    tp_PlaylistWindow->show();
    tp_PlaylistWindow->move(globalPositionBottomLeft_PlaylistWindow);
}

// *****************************************************************
// private
// *****************************************************************

void
TP_MainClass::connect()
{

}
