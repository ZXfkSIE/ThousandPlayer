﻿#include "tp_lyricsstackedwidget.h"

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
         && cursorPositionType != TP::CursorPositionType::NonBorder )
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
        case TP::CursorPositionType::Left :

            newGeometry.setLeft( event->globalPosition().toPoint().x() );
            if( newGeometry.width() < window()->minimumWidth() )
                newGeometry.setLeft( newGeometry.right() - window()->minimumWidth() + 1 );

            if( newGeometry != window()->geometry() )
                emit signal_resizeWindow( newGeometry, TP::CursorPositionType::Left );

            break;

        case TP::CursorPositionType::Right :

            newGeometry.setRight( event->globalPosition().toPoint().x() );
            if( newGeometry.width() < window()->minimumWidth() )
                newGeometry.setWidth( window()->minimumWidth() );

            if( newGeometry != window()->geometry() )
                emit signal_resizeWindow( newGeometry, TP::CursorPositionType::Right );

            break;

        case TP::CursorPositionType::Bottom :

            newGeometry.setBottom( event->globalPosition().toPoint().y() );
            if( newGeometry.height() < window()->minimumHeight() )
                newGeometry.setHeight( window()->minimumHeight() );

            if( newGeometry != window()->geometry() )
                emit signal_resizeWindow( newGeometry, TP::CursorPositionType::Bottom );

            break;

        default:
            break;
        }
    }
    else                        // if ( ! b_isBorderBeingPressed )
    {
        cursorPositionType = TP::getCursorPositionType( this, eventPosition );
        switch ( cursorPositionType )
        {
        case TP::CursorPositionType::NonBorder:
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

        case TP::CursorPositionType::Bottom :
            if ( ! b_isCursorResize )
            {
                setCursor( QCursor( Qt::SizeVerCursor ) );
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
        if( TP::getCursorPositionType( this, event->position().toPoint() ) == TP::CursorPositionType::NonBorder
                && b_isCursorResize )
        {
            setCursor( QCursor( Qt::ArrowCursor ) );
            b_isCursorResize = false;
        }
        emit signal_windowChanged();
    }

    QStackedWidget::mouseReleaseEvent( event );
}
