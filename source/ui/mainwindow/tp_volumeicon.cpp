#include "tp_volumeicon.h"

#include <QIcon>
#include <QMouseEvent>

TP_VolumeIcon::TP_VolumeIcon( QWidget *parent ) :
    QLabel          { parent }
  , originalVolume  { 50 }
{

}

// Must be executed manually after the initialization of UI
// to get the correct width and height.
void
TP_VolumeIcon::initialize()
{
    pixmap_Volume = QIcon( ":/image/icon_Volume.svg" )
            .pixmap( QSize( width(), height() ) );
    pixmap_Mute = QIcon( ":/image/icon_Mute.svg" )
            .pixmap( QSize( width(), height() ) );
}


void
TP_VolumeIcon::setIcon( int I_volume )
{
    volume = I_volume;
    if( volume )
        setPixmap( pixmap_Volume );
    else
        setPixmap( pixmap_Mute );
}


// *****************************************************************
// private override
// *****************************************************************

void
TP_VolumeIcon::mousePressEvent( QMouseEvent *event )
{
    if( event->button() == Qt::LeftButton )
    {
        if( volume )
        {
            originalVolume = volume;
            emit signal_setVolume( 0 );
        }
        else
        {
            if( ! originalVolume )
                originalVolume = 50;
            emit signal_setVolume( originalVolume );
        }
    }

    QLabel::mousePressEvent( event );
}
