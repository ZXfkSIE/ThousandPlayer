#include "tp_volumeslider.h"

#include <QMouseEvent>

TP_VolumeSlider::TP_VolumeSlider( QWidget *parent ) :
    QSlider( parent )
  , b_invertMouseWheel { false }
{
    setRange(0, 100);
}

void
TP_VolumeSlider::setInvertMouseWheel( bool b )
{
    b_invertMouseWheel = b;
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_VolumeSlider::mouseReleaseEvent( QMouseEvent *event )
{
    if( event->button() == Qt::LeftButton )
    {
        int second = minimum() +
                ( ( maximum() - minimum() ) * event->position().x() ) / width();
        setValue( second );
    }
    QSlider::mouseReleaseEvent( event );
}

void
TP_VolumeSlider::wheelEvent (QWheelEvent *event)
{
    if( b_invertMouseWheel )
    {
        QPoint newAngleDelta = event->angleDelta();
        newAngleDelta.setX( -newAngleDelta.x() );
        newAngleDelta.setY( -newAngleDelta.y() );
        QWheelEvent *newEvent = new QWheelEvent
        {
            event->position(), event->globalPosition(), event->pixelDelta(),
            newAngleDelta,
            event->buttons(), event->modifiers(), event->phase(), event->inverted(), event->source()
        };
        QSlider::wheelEvent( newEvent );
        event->ignore();
    }
    else
        QSlider::wheelEvent( event );
}
