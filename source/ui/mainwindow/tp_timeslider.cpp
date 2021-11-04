#include "tp_timeslider.h"

#include <QMouseEvent>

TP_TimeSlider::TP_TimeSlider( QWidget *parent ) :
    QSlider             { parent }
{
    setRange( 0, 0 );
}

// *****************************************************************
// private override
// *****************************************************************

// It is not favorable to change the time position every time when valueChanged().
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
TP_TimeSlider::wheelEvent( QWheelEvent *event )
{
    emit signal_mouseReleased( value() );
    QSlider::wheelEvent( event );
}
