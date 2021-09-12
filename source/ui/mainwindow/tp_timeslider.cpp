#include "tp_timeslider.h"

#include <QMouseEvent>

TP_TimeSlider::TP_TimeSlider( QWidget *parent ) :
    QSlider             { parent }
  , b_invertMouseWheel  { false }
{
    setRange(0, 0);
}

void
TP_TimeSlider::setInvertMouseWheel( bool b )
{
    b_invertMouseWheel = b;
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_TimeSlider::mouseReleaseEvent( QMouseEvent *event )
{
    if( event->button() == Qt::LeftButton && maximum() )
    {
        int second = minimum() +
                ( ( maximum() - minimum() ) * event->position().x() ) / width();
        setValue( second );
        emit signal_mouseReleased( second );
    }
    QSlider::mouseReleaseEvent( event );
}

void
TP_TimeSlider::wheelEvent (QWheelEvent *event)
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
    }
    else
        QSlider::wheelEvent( event );

    emit signal_mouseReleased( value() );
}
