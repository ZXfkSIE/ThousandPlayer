#include "tp_lyricsstackedwidget.h"

#include "tp_globalconst.h"
#include "tp_globalfunction.h"

#include <QMouseEvent>

TP_LyricsStackedWidget::TP_LyricsStackedWidget( QWidget *parent ) :
    QStackedWidget          { parent }
  , b_isBorderBeingPressed  { false }
  , b_isCursorResize        { false }
{

}

// *****************************************************************
// private override
// *****************************************************************

void
TP_LyricsStackedWidget::mousePressEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton
         && cursorPositionType != TP::notAtBorder )
        b_isBorderBeingPressed = true;

    QStackedWidget::mousePressEvent( event );
}

void
TP_LyricsStackedWidget::mouseMoveEvent( QMouseEvent *event )
{
    QPoint eventPosition = event->position().toPoint();

    if ( b_isBorderBeingPressed )
    {
        auto newGeometry = window()->geometry();

        switch ( cursorPositionType )
        {
        case TP::leftBorder:

            newGeometry.setLeft( event->globalPosition().toPoint().x() );
            if( newGeometry.width() < window()->minimumWidth() )
                newGeometry.setLeft( newGeometry.right() - window()->minimumWidth() + 1 );

            if( newGeometry != window()->geometry() )
                emit signal_resizeWindow( newGeometry, TP::atLeft );

            break;

        case TP::rightBorder:

            newGeometry.setRight( event->globalPosition().toPoint().x() );
            if( newGeometry.width() < window()->minimumWidth() )
                newGeometry.setWidth( window()->minimumWidth() );

            if( newGeometry != window()->geometry() )
                emit signal_resizeWindow( newGeometry, TP::atRight );

            break;

        case TP::bottomBorder:

            newGeometry.setBottom( event->globalPosition().toPoint().y() );
            if( newGeometry.height() < window()->minimumHeight() )
                newGeometry.setHeight( window()->minimumHeight() );

            if( newGeometry != window()->geometry() )
                emit signal_resizeWindow( newGeometry, TP::atBottom );

            break;

        default:
            break;
        }
    }
    else                        // if (b_isBorderBeingPressed)
    {
        cursorPositionType = TP::getCursorPositionType( this, eventPosition );
        switch ( cursorPositionType )
        {
        case TP::notAtBorder:
            if ( b_isCursorResize )
            {
                setCursor( QCursor( Qt::ArrowCursor ) );
                b_isCursorResize = false;
            }
            break;

        case TP::leftBorder:
        case TP::rightBorder:
        case TP::bottomBorder:
            if ( ! b_isCursorResize )
            {
                setCursor( QCursor( Qt::SizeHorCursor ) );
                b_isCursorResize = true;
            }
            break;

        default:
            break;
        }
    }                           // if (b_isBorderBeingPressed)

    QStackedWidget::mouseMoveEvent( event );
}

void
TP_LyricsStackedWidget::mouseReleaseEvent( QMouseEvent *event )
{
    if( b_isBorderBeingPressed )
    {
        b_isBorderBeingPressed = false;
        if( TP::getCursorPositionType( this, event->position().toPoint() ) == TP::notAtBorder
                && b_isCursorResize )
        {
            setCursor( QCursor( Qt::ArrowCursor ) );
            b_isCursorResize = false;
        }
        emit signal_windowChanged();
    }

    QStackedWidget::mouseReleaseEvent( event );
}
