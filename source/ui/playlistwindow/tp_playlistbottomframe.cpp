#include "tp_playlistbottomframe.h"

#include "tp_globalconst.h"

#include <QMouseEvent>

TP_PlaylistBottomFrame::TP_PlaylistBottomFrame( QWidget *parent ) :
    QFrame { parent }
  , b_isBorderBeingPressed { false }
  , b_isCursorResize { false }
  , b_isExpandingDisabled { false }
{

}

// *****************************************************************
// private override
// *****************************************************************

void
TP_PlaylistBottomFrame::mousePressEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton && cursorPositionType )
    {
        b_isBorderBeingPressed = true;
        b_isExpandingDisabled = false;
    }

    QFrame::mousePressEvent( event );
}

void
TP_PlaylistBottomFrame::mouseMoveEvent( QMouseEvent *event )
{
    QPoint eventPosition = event->position().toPoint();

    if ( b_isBorderBeingPressed )
    {
        QRect newGeometry = window()->geometry();

        if( cursorPositionType == TP::bottomBorder )
        {
            newGeometry.setBottom( event->globalPosition().toPoint().y() );
            if( newGeometry.height() < window()->minimumHeight() )
                newGeometry.setHeight( window()->minimumHeight() );

            if( newGeometry != geometry() )
                emit signal_resizeWindow( newGeometry, TP::atBottom );
        }
    }
    else
    {
        cursorPositionType = isAtBorder( eventPosition );
        switch ( cursorPositionType )
        {
        case TP::notAtBorder:
            if ( b_isCursorResize )
            {
                setCursor( QCursor( Qt::ArrowCursor ) );
                b_isCursorResize = false;
            }
            break;

        case TP::bottomBorder:
            if ( ! b_isCursorResize )
            {
                setCursor( QCursor( Qt::SizeVerCursor ) );
                b_isCursorResize = true;
            }

            break;

        default:
            break;
        }
    }

    QFrame::mouseMoveEvent( event );
}

void
TP_PlaylistBottomFrame::mouseReleaseEvent( QMouseEvent *event )
{
    b_isBorderBeingPressed = false;
    b_isExpandingDisabled = false;
    if( !isAtBorder( event->position().toPoint() ) && b_isCursorResize )
    {
        setCursor( QCursor( Qt::ArrowCursor ) );
        b_isCursorResize = false;
    }
    emit signal_leftButtonReleased();

    QFrame::mouseReleaseEvent( event );
}

// *****************************************************************
// private
// *****************************************************************

TP::CursorPositionType
TP_PlaylistBottomFrame::isAtBorder( const QPoint &I_point ) const
{
    if ( height() - I_point.y() <= TP::borderSize )
    {
        return TP::bottomBorder;
    }
    return TP::notAtBorder;
}
