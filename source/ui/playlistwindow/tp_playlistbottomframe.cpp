﻿#include "tp_playlistbottomframe.h"

#include "tp_globalconst.h"
#include "tp_globalfunction.h"

#include <QMouseEvent>

TP_PlaylistBottomFrame::TP_PlaylistBottomFrame( QWidget *parent ) :
    QFrame                  { parent }
  , b_isBorderBeingPressed  { false }
  , b_isCursorResize        { false }
{

}

// *****************************************************************
// private override
// *****************************************************************

void
TP_PlaylistBottomFrame::mousePressEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton
         && cursorPositionType != TP::CursorPositionType::NonBorder )
        b_isBorderBeingPressed = true;

    QFrame::mousePressEvent( event );
}

void
TP_PlaylistBottomFrame::mouseMoveEvent( QMouseEvent *event )
{
    QPoint eventPosition = event->position().toPoint();

    if ( b_isBorderBeingPressed )
    {
        auto newGeometry = window()->geometry();

        if( cursorPositionType == TP::CursorPositionType::Bottom )
        {
            newGeometry.setBottom( event->globalPosition().toPoint().y() );
            if( newGeometry.height() < window()->minimumHeight() )
                newGeometry.setHeight( window()->minimumHeight() );

            if( newGeometry != window()->geometry() )
                emit signal_resizeWindow( newGeometry, TP::CursorPositionType::Bottom );
        }
    }
    else
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
    }

    QFrame::mouseMoveEvent( event );
}

void
TP_PlaylistBottomFrame::mouseReleaseEvent( QMouseEvent *event )
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

    QFrame::mouseReleaseEvent( event );
}
