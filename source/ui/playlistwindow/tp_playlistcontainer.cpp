#include "tp_playlistcontainer.h"

#include "tp_globalconst.h"

#include <QMouseEvent>

TP_PlaylistContainer::TP_PlaylistContainer( QWidget *parent ) :
    QWidget { parent }
  , b_isBorderBeingPressed { false }
  , b_isCursorResize { false }
  , b_isExpandingDisabled { false }
{

}


// *****************************************************************
// private override
// *****************************************************************

void
TP_PlaylistContainer::mousePressEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton && cursorPositionType )
    {
        b_isBorderBeingPressed = true;
        b_isExpandingDisabled = false;
        pressedGlobalPosition = event->globalPosition().toPoint();
    }

    QWidget::mousePressEvent( event );
}

void
TP_PlaylistContainer::mouseMoveEvent( QMouseEvent *event )
{
    QPoint eventPosition = event->position().toPoint();

    if ( b_isBorderBeingPressed )
    {
        int differenceX = event->globalPosition().toPoint().x() - pressedGlobalPosition.x();
        QRect newGeometry = window()->geometry();

        switch ( cursorPositionType )
        {
        case TP::leftBorder:
            if ( differenceX < 0 && b_isExpandingDisabled )
                return;

            newGeometry.setLeft( newGeometry.left() + differenceX );
            if( newGeometry.width() >= window()->minimumWidth() )
                emit signal_resizeWindow( newGeometry, TP::atLeft );
            pressedGlobalPosition = event->globalPosition().toPoint();
            if ( event->position().toPoint().x() > width() - window()->minimumWidth() + TP::borderSize )
                b_isExpandingDisabled = true;

            break;              // case TP_LEFT_BORDER

        case TP::rightBorder:
            if ( differenceX > 0 && b_isExpandingDisabled )
                return;

            newGeometry.setRight( newGeometry.right() + differenceX );
            emit signal_resizeWindow( newGeometry, TP::atRight );
            pressedGlobalPosition = event->globalPosition().toPoint();
            if ( event->position().toPoint().x() < width() -  TP::borderSize )
                b_isExpandingDisabled = true;

            break;              // case TP_RIGHT_BORDER

        default:
            break;
        }
    }
    else                        // if (b_isBorderBeingPressed)
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

        case TP::leftBorder:
        case TP::rightBorder:
            if (! b_isCursorResize)
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
    b_isBorderBeingPressed = false;
    b_isExpandingDisabled = false;
    if( !isAtBorder( event->position().toPoint() ) && b_isCursorResize )
    {
        setCursor( QCursor( Qt::ArrowCursor ) );
        b_isCursorResize = false;
    }

    QWidget::mouseReleaseEvent( event );
}

// *****************************************************************
// private
// *****************************************************************

TP::CursorPositionType
TP_PlaylistContainer::isAtBorder( const QPoint &I_point ) const
{
    if ( I_point.x() <= TP::borderSize )
    {
        return TP::leftBorder;
    }
    else if ( width() - I_point.x() <= TP::borderSize )
    {
        return TP::rightBorder;
    }
    return TP::notAtBorder;
}
