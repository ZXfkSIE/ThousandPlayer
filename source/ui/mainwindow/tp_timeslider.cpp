#include "tp_timeslider.h"

#include "tp_timesliderproxystyle.h"

#include <QMouseEvent>
#include <QStyle>

TP_TimeSlider::TP_TimeSlider( QWidget *parent ) :
    QSlider             { parent }
{
    setRange( 0, 0 );

    setStyle( new TP_TimeSliderProxyStyle { style() } );
}

// *****************************************************************
// private override
// *****************************************************************

// It is not favorable to change the time position every time when valueChanged().
void
TP_TimeSlider::mouseReleaseEvent( QMouseEvent *event )
{
    QSlider::mouseReleaseEvent( event );
    if( event->button() == Qt::LeftButton && maximum() )
        emit signal_mouseReleased( value() );
}

void
TP_TimeSlider::wheelEvent( QWheelEvent *event )
{
    QSlider::wheelEvent( event );
    emit signal_mouseReleased( value() );
}
