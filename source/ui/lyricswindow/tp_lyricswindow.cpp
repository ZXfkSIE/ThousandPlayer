#include "tp_lyricswindow.h"
#include "ui_tp_lyricswindow.h"

#include "tp_globalconst.h"
#include "tp_globalvariable.h"

#include "tp_lyricsviewer.h"

TP_LyricsWindow::TP_LyricsWindow( QWidget *parent ) :
    QWidget         { parent }
  , ui              { new Ui::TP_LyricsWindow }
  , lyricsViewer    { new TP_LyricsViewer { this } }
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
TP_LyricsWindow::readLrcFileFromCurrentItem()
{
    auto qstr_localFilePath { TP::currentItem()->data( TP::role_URL ).toUrl().toLocalFile() };
    auto idx_LastPositionOfPoint { qstr_localFilePath.lastIndexOf( '.' ) };
    auto qstr_lrcPath { qstr_localFilePath.first( ++idx_LastPositionOfPoint ) + QString { "lrc" } };
    lyricsViewer->readLrcFile( qstr_lrcPath );
}


void
TP_LyricsWindow::clearLrcFile()
{
    lyricsViewer->readLrcFile( {} );
}


void
TP_LyricsWindow::updatePosition( qint64 I_ms )
{
    lyricsViewer->updatePosition( I_ms );
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
TP_LyricsWindow::slot_changeFont()
{
    lyricsViewer->changeFont( TP::config().getLyricsFont() );
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

    // Lyrics viewer related
    connect( lyricsViewer,  &TP_LyricsViewer::signal_lyricsDoubleClicked,
             this,          &TP_LyricsWindow::slot_lyricsDoubleClicked );
}

void
TP_LyricsWindow::initializeUI()
{
    lyricsViewer->readLrcFile( {} );
    ui->page_LyricsViewer->layout()->addWidget( lyricsViewer );
    ui->lyricsStackedWidget->setCurrentWidget( ui->page_LyricsViewer );
    slot_changeFont();
}
