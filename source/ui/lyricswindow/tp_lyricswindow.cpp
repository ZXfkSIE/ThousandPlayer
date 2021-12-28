#include "tp_lyricswindow.h"
#include "ui_tp_lyricswindow.h"

#include "tp_globalvariable.h"

TP_LyricsWindow::TP_LyricsWindow( QWidget *parent ) :
    QWidget { parent },
    ui      { new Ui::TP_LyricsWindow }
{
    ui->setupUi( this );
}

TP_LyricsWindow::~TP_LyricsWindow()
{
    delete ui;
}

// *****************************************************************
// private slots
// *****************************************************************

void
TP_LyricsWindow::slot_resizeWindow( const QRect &newGeomtry, TP::ResizeType resizeType )
{
    emit signal_resizeWindow( this, newGeomtry, resizeType );
}


void
TP_LyricsWindow::slot_titleBarMoved( const QRect &newGeometry )
{
    emit signal_moveWindow( this, newGeometry );
}


void
TP_LyricsWindow::slot_windowChanged()
{
    emit signal_windowChanged();
}


void
TP_LyricsWindow::slot_changeFontOfWidgets()
{
   // const auto &font { TP::config().getLyricsFont() };
}

// *****************************************************************
// private
// *****************************************************************

void
TP_LyricsWindow::initializeConnection()
{
    // Window moving and resizing related
    connect( ui->frame_Title,           &TP_TitleBar::signal_moveTitleBar,
             this,                      &TP_LyricsWindow::slot_titleBarMoved );
    connect( ui->frame_Title,           &TP_TitleBar::signal_leftButtonReleased,
             this,                      &TP_LyricsWindow::slot_windowChanged );
    connect( ui->lyricsStackedWidget,   &TP_LyricsStackedWidget::signal_resizeWindow,
             this,                      &TP_LyricsWindow::slot_resizeWindow );
    connect( ui->lyricsStackedWidget,   &TP_LyricsStackedWidget::signal_windowChanged,
             this,                      &TP_LyricsWindow::slot_windowChanged );
}
