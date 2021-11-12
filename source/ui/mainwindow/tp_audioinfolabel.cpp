#include "tp_audioinfolabel.h"

#include "tp_globalvariable.h"

#include <QMouseEvent>
#include <QPainter>
#include <QTimer>


TP_AudioInfoLabel::TP_AudioInfoLabel( QWidget *parent ) :
    QLabel { parent }
  , vec_Qstr {}
  , timer { new QTimer{ this } }
  , b_isScrolling { false }
  , b_isTimerTriggered { false }
{
    connect( timer, &QTimer::timeout,
             this,  &TP_AudioInfoLabel::slot_timerIsTriggered );
}

// Must be executed manually after the window is shown
void
TP_AudioInfoLabel::initialize()
{
    initialX = contentsRect().x() + 5;
    initialMiddleY = height() >> 1;
}

void
TP_AudioInfoLabel::setStrings( std::vector< QString > &&I_vec_Qstr )
{
    vec_Qstr = std::move( I_vec_Qstr );
    b_isScrolling = false;

    if( vec_Qstr.size() <= 1 )
        timer->stop();
    else
        timer->start( TP::config().getAudioInfoScrollingInterval() * 1000 );

    b_isTimerTriggered = false;
    currentIdx = 0;
    repaint();
}

// *****************************************************************
// private slots
// *****************************************************************

void
TP_AudioInfoLabel::slot_timerIsTriggered()
{
    b_isTimerTriggered = true;
    repaint();
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_AudioInfoLabel::paintEvent ( QPaintEvent * )
{
    QLabel::paintEvent( {} );
    QPainter painter { this };
    painter.setClipRect( contentsRect() );

    if( b_isTimerTriggered )
    {
        if( b_isScrolling )
        {
            int offsetY { font().pointSize() >> 1 };
            int nextIdx = nextIndex();

            // draw the upper text
            painter.drawText( x, --y + offsetY , vec_Qstr[ currentIdx ] );

            // draw the lower text
            painter.drawText( x, y + height() + offsetY, vec_Qstr[ nextIdx ] );

            // if the lower text has been moved to the initial position
            if( y + height() == initialMiddleY )
            {
                b_isScrolling = false;
                timer->start( TP::config().getAudioInfoScrollingInterval() * 1000 );
                currentIdx = nextIdx;
            }
        }
        else        // not scrolling, start scrolling
        {
            painter.drawText( initialX, initialMiddleY + ( font().pointSize() >> 1 ), vec_Qstr[ currentIdx ] );
            x = initialX;
            y = initialMiddleY;
            b_isScrolling = true;
            timer->start( 50 );             // 20 fps
        }

        b_isTimerTriggered = false;
    }
    else
        painter.drawText( initialX, initialMiddleY + ( font().pointSize() >> 1 ), vec_Qstr[ currentIdx ] );
}


void
TP_AudioInfoLabel::mousePressEvent( QMouseEvent *event )
{
    // For unknown reason, mouse click ruins text drawing under Linux.
#ifdef Q_OS_WIN
    if( event->button() == Qt::LeftButton )
    {
        if( vec_Qstr.size() <= 1 )
            timer->stop();
        else
            timer->start( TP::config().getAudioInfoScrollingInterval() * 1000 );

        b_isTimerTriggered = false;
        currentIdx = nextIndex();
        repaint();
    }
#endif
    QLabel::mousePressEvent( event );
}

// *****************************************************************
// private
// *****************************************************************

int
TP_AudioInfoLabel::nextIndex()
{
    return ( currentIdx == vec_Qstr.size() - 1 ) ? 0 : currentIdx + 1;
}
