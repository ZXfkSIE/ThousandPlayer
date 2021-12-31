#include "tp_lyricswindow.h"
#include "ui_tp_lyricswindow.h"

#include "tp_globalvariable.h"

#include "tp_lyricsviewer.h"

TP_LyricsWindow::TP_LyricsWindow( QWidget *parent ) :
    QWidget         { parent }
  , ui              { new Ui::TP_LyricsWindow }
  , lyricsViewer    { new TP_LyricsViewer { ui->lyricsStackedWidget } }
{
    ui->setupUi( this );

    initializeConnection();
    initializeUI();
}

TP_LyricsWindow::~TP_LyricsWindow()
{
    delete ui;
}

// *****************************************************************
// public slots
// *****************************************************************

void
TP_LyricsWindow::slot_changeFont()
{
    lyricsViewer->setFont( TP::config().getLyricsFont() );
}

// *****************************************************************
// private slots
// *****************************************************************

void
TP_LyricsWindow::slot_resizeWindow( const QRect &I_geometry, TP::ResizeType I_resizeType )
{
    emit signal_resizeWindow( this, I_geometry, I_resizeType );
}


void
TP_LyricsWindow::slot_titleBarMoved( const QRect &I_geometry )
{
    emit signal_moveWindow( this, I_geometry );
}


void
TP_LyricsWindow::slot_windowChanged()
{
    emit signal_windowChanged();
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_LyricsWindow::showEvent( QShowEvent *event )
{
    QWidget::showEvent( event );
    emit signal_shown();
}


void
TP_LyricsWindow::hideEvent( QHideEvent *event )
{
    QWidget::hideEvent( event );
    emit signal_hidden();
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

void
TP_LyricsWindow::initializeUI()
{
    ui->lyricsStackedWidget->setCurrentWidget( lyricsViewer );
    slot_changeFont();
}
