#include "tp_playlistcontainer.h"

#include "tp_globalfunction.h"

#include <QMouseEvent>

TP_PlaylistContainer::TP_PlaylistContainer( QWidget *parent ) :
    QWidget                 { parent }
  , b_isBorderBeingPressed  { false }
  , b_isCursorResize        { false }
{

}

// *****************************************************************
// private override
// *****************************************************************

void
TP_PlaylistContainer::mousePressEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton
         && cursorPositionType != TP::CursorPositionType::NonBorder )
        b_isBorderBeingPressed = true;

    QWidget::mousePressEvent( event );
}

void
TP_PlaylistContainer::mouseMoveEvent( QMouseEvent *event )
{
    QPoint eventPosition = event->position().toPoint();

    if ( b_isBorderBeingPressed )
    {
        auto newGeometry = window()->geometry();

        switch ( cursorPositionType )
        {
        case TP::CursorPositionType::Left :

            newGeometry.setLeft( event->globalPosition().toPoint().x() );
            if( newGeometry.width() < window()->minimumWidth() )
                newGeometry.setLeft( newGeometry.right() - window()->minimumWidth() + 1 );

            if( newGeometry != window()->geometry() )
                emit signal_resizeWindow( newGeometry, TP::CursorPositionType::Left );

            break;              // case TP_LEFT_BORDER

        case TP::CursorPositionType::Right :

            newGeometry.setRight( event->globalPosition().toPoint().x() );
            if( newGeometry.width() < window()->minimumWidth() )
                newGeometry.setWidth( window()->minimumWidth() );

            if( newGeometry != window()->geometry() )
                emit signal_resizeWindow( newGeometry, TP::CursorPositionType::Right );

            break;              // case TP_RIGHT_BORDER

        default:
            break;
        }
    }
    else                        // if (b_isBorderBeingPressed)
    {
        cursorPositionType = TP::getCursorPositionType( this, eventPosition );
        switch ( cursorPositionType )
        {
        case TP::CursorPositionType::NonBorder :
            if ( b_isCursorResize )
            {
                setCursor( QCursor( Qt::ArrowCursor ) );
                b_isCursorResize = false;
            }
            break;

        case TP::CursorPositionType::Left :
        case TP::CursorPositionType::Right :
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

    QWidget::mouseMoveEvent( event );
}

void
TP_PlaylistContainer::mouseReleaseEvent( QMouseEvent *event )
{
    if( b_isBorderBeingPressed )
    {
        b_isBorderBeingPressed = false;
        if( TP::getCursorPositionType( this, event->position().toPoint() ) == TP::CursorPositionType::NonBorder
                && b_isCursorResize )
        {
            setCursor( QCursor( Qt::ArrowCursor ) );
            b_isCursorResize = false;
        }
        emit signal_windowChanged();
    }

    QWidget::mouseReleaseEvent( event );
}
