#include "tp_coverviewer.h"
#include "ui_tp_coverviewer.h"

#include <QStyle>
#include <QWheelEvent>

TP_CoverViewer::TP_CoverViewer( QDialog *parent ) :
    QDialog                 { parent }
  , ui                      { new Ui::TP_CoverViewer }
  , pixmap                  {}
  , scaleFactor             { 1.0f }
  , b_isMaximized           { false }
{
    ui->setupUi( this );
    originalGeometry = geometry();

    setWindowFlags( Qt::Tool |
                    Qt::WindowMaximizeButtonHint |
                    Qt::WindowCloseButtonHint );
    setWindowIcon( QIcon{ ":/image/icon_CoverViewer.svg" } );

    ui->label_Image->setBackgroundRole( QPalette::Base );
    ui->label_Image->setAlignment( Qt::AlignCenter );
    ui->label_Image->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    ui->pushButton_ZoomIn       ->setIcon( QIcon{ ":/image/icon_ZoomIn.svg" } );
    ui->pushButton_ZoomOut      ->setIcon( QIcon{ ":/image/icon_ZoomOut.svg" } );
    ui->pushButton_OriginalSize ->setIcon( QIcon{ ":/image/icon_OriginalSize.svg" } );
    ui->pushButton_Maximize     ->setIcon( QIcon{ ":/image/icon_Maximize.svg" } );

#ifdef Q_OS_WIN
    ui->pushButton_Maximize->hide();
#endif
}


TP_CoverViewer::~TP_CoverViewer()
{
    delete ui;
}


void
TP_CoverViewer::setImage( const QPixmap &I_pixmap )
{
    if( I_pixmap.isNull() )
        pixmap = {};
    else
    {
        pixmap = I_pixmap;
        on_pushButton_OriginalSize_clicked();
    }
}

// *****************************************************************
// private slots
// *****************************************************************

void
TP_CoverViewer::on_pushButton_ZoomIn_clicked()
{
    scaleImage( 1.25f );
}


void
TP_CoverViewer::on_pushButton_ZoomOut_clicked()
{
    scaleImage( 0.8f );
}


void
TP_CoverViewer::on_pushButton_OriginalSize_clicked()
{
    scaleFactor = 1.0f;
    scaleImage( 1.0f );
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_CoverViewer::wheelEvent( QWheelEvent *event )
{
    if( event->angleDelta().y() > 0 && ui->pushButton_ZoomIn->isEnabled() )
        on_pushButton_ZoomIn_clicked();
    else if( event->angleDelta().y() < 0 && ui->pushButton_ZoomOut->isEnabled() )
        on_pushButton_ZoomOut_clicked();

    QWidget::wheelEvent( event );
}

// *****************************************************************
// private
// *****************************************************************

void
TP_CoverViewer::scaleImage( float I_multiplier )
{
    scaleFactor *= I_multiplier;
    ui->label_Image->setPixmap( pixmap.scaled(
                                    pixmap.size() * scaleFactor,
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation
                                    )
                                );
    ui->pushButton_ZoomIn->setEnabled( scaleFactor <= 4.0f );
    ui->pushButton_ZoomOut->setEnabled( scaleFactor >= 0.25f );
}


void
TP_CoverViewer::on_pushButton_Maximize_clicked()
{
    if( b_isMaximized )
    {
        setGeometry( originalGeometry );
        ui->pushButton_Maximize->setIcon( QIcon{ ":/image/icon_Maximize.svg" } );
        ui->pushButton_Maximize->setToolTip( tr( "Maximize" ) );
        b_isMaximized = false;
    }
    else
    {
        originalGeometry = geometry();
        setGeometry( QApplication::screenAt( pos() )->availableGeometry() );
        ui->pushButton_Maximize->setIcon( QIcon{ ":/image/icon_Restore.svg" } );
        ui->pushButton_Maximize->setToolTip( tr( "Minimize" ) );
        b_isMaximized = true;
    }

}

