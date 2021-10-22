#include "tp_coverviewer.h"
#include "ui_tp_coverviewer.h"

#include <QStyle>

TP_CoverViewer::TP_CoverViewer( QWidget *parent ) :
    QWidget     { parent }
  , ui          { new Ui::TP_CoverViewer }
  , pixmap      {}
  , scaleFactor { 1.0f }
{
    ui->setupUi( this );
    setWindowTitle( QString("ThousandPlayer - ") + tr("Cover Viewer") );
    setWindowIcon( QIcon{":/image/icon_CoverViewer.svg"} );

    ui->label_Image->setBackgroundRole( QPalette::Base );
    ui->label_Image->setAlignment( Qt::AlignCenter );
    ui->label_Image->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    ui->pushButton_ZoomIn->setIcon( QIcon{":/image/icon_ZoomIn.svg"} );
    ui->pushButton_ZoomOut->setIcon( QIcon{":/image/icon_ZoomOut.svg"} );
    ui->pushButton_OriginalSize->setIcon( QIcon{":/image/icon_OriginalSize.svg"} );
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



