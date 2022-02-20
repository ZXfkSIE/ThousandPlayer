#include "tp_lyricswindow.h"
#include "ui_tp_lyricswindow.h"

#include "tp_globalconst.h"
#include "tp_globalfunction.h"
#include "tp_globalvariable.h"

#include "tp_lyricseditor.h"
#include "tp_lyricsviewer.h"

TP_LyricsWindow::TP_LyricsWindow( QWidget *parent ) :
    QWidget         { parent }
  , ui              { new Ui::TP_LyricsWindow }
  , lyricsViewer    { new TP_LyricsViewer { this } }
  , lyricsEditor    { new TP_LyricsEditor { this } }
{
    ui->setupUi( this );
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint | Qt::Tool | Qt::NoDropShadowWindowHint );
    ui->pushButton_Close->setIcon( QIcon { ":/image/icon_Exit.svg" } );

    initializeConnection();
    initializeUI();
}


TP_LyricsWindow::~TP_LyricsWindow()
{
    delete ui;
}


void
TP_LyricsWindow::readLyricsFileFromCurrentItem()
{
    lyricsViewer->readLyricsFile( TP::getLyricsURL( TP::currentItem() ) );
}


void
TP_LyricsWindow::clearLyricsViewer()
{
    lyricsViewer->readLyricsFile( {} );
}


void
TP_LyricsWindow::updatePosition( qint64 I_ms )
{
    lyricsViewer->updatePosition( I_ms );
    lyricsEditor->setCurrentPosition( I_ms );
}

// *****************************************************************
// public slots
// *****************************************************************

void
TP_LyricsWindow::slot_activateWindow()
{
    if( isVisible() )
        raise();
}


void
TP_LyricsWindow::slot_refreshFont()
{
    lyricsViewer->refreshFont();
    lyricsEditor->refreshFont();
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


void
TP_LyricsWindow::slot_lyricsDoubleClicked( qint64 I_ms )
{
    emit signal_lyricsDoubleClicked( I_ms );
}


void
TP_LyricsWindow::slot_switchToLyricsViewer( const QUrl &I_url )
{
    lyricsViewer->reloadLyricsFile( I_url );
    ui->lyricsStackedWidget->setCurrentWidget( ui->page_LyricsViewer );
}


void
TP_LyricsWindow::slot_switchToLyricsEditor( const QUrl &I_url )
{
    lyricsEditor->readLyricsFile( I_url );
    ui->lyricsStackedWidget->setCurrentWidget( ui->page_LyricsEditor );
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

    // Lyrics viewer & editor related
    connect( lyricsViewer,  &TP_LyricsViewer::signal_lyricsDoubleClicked,
             this,          &TP_LyricsWindow::slot_lyricsDoubleClicked );
    connect( lyricsViewer,  &TP_LyricsViewer::signal_switchToLyricsEditor,
             this,          &TP_LyricsWindow::slot_switchToLyricsEditor );
    connect( lyricsEditor,  &TP_LyricsEditor::signal_switchToLyricsViewer,
             this,          &TP_LyricsWindow::slot_switchToLyricsViewer );
}

void
TP_LyricsWindow::initializeUI()
{
    lyricsViewer->readLyricsFile( {} );

    ui->page_LyricsViewer->layout()->addWidget( lyricsViewer );
    ui->page_LyricsEditor->layout()->addWidget( lyricsEditor );

    ui->lyricsStackedWidget->setCurrentWidget( ui->page_LyricsViewer );

    slot_refreshFont();
}
