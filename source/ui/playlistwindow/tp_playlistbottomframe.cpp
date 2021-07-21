#include "tp_playlistbottomframe.h"

#include "tp_globalconst.h"

#include <QMouseEvent>

TP_PlaylistBottomFrame::TP_PlaylistBottomFrame(QWidget *parent) :
    QFrame(parent)
  , b_isBorderBeingPressed(false)
  , b_isCursorResize(false)
  , b_isExpandingDisabled(false)
  , cursorPositionType(0)
{
    setMouseTracking(true);
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_PlaylistBottomFrame::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && cursorPositionType)
    {
        b_isBorderBeingPressed = true;
        b_isExpandingDisabled = false;
        pressedGlobalPosition = event->globalPosition().toPoint();
    }

    QFrame::mousePressEvent(event);
}

void
TP_PlaylistBottomFrame::mouseMoveEvent(QMouseEvent *event)
{
    QPoint eventPosition = event->position().toPoint();

    if (b_isBorderBeingPressed)
    {
        int differenceY = event->globalPosition().toPoint().y() - pressedGlobalPosition.y();
        QRect rect_Geometry = window()->geometry();

        if(cursorPositionType == TP_BOTTOM_BORDER)
        {
            if (differenceY > 0 && b_isExpandingDisabled)
                return;

            rect_Geometry.setBottom(rect_Geometry.bottom() + differenceY);
            window()->setGeometry(rect_Geometry);
            pressedGlobalPosition = event->globalPosition().toPoint();
            if (event->position().toPoint().y() < height() -  TP_BORDER_SIZE)
                b_isExpandingDisabled = true;
        }
    }
    else
    {
        cursorPositionType = isAtBorder(eventPosition);
        switch (cursorPositionType)
        {
        case 0:
            if (b_isCursorResize)
            {
                setCursor(QCursor(Qt::ArrowCursor));
                b_isCursorResize = false;
            }
            break;

        case TP_BOTTOM_BORDER:
            if (! b_isCursorResize)
            {
                setCursor(QCursor(Qt::SizeVerCursor));
                b_isCursorResize = true;
            }

            break;
        }
    }

    QFrame::mouseMoveEvent(event);
}

void
TP_PlaylistBottomFrame::mouseReleaseEvent(QMouseEvent *event)
{
    b_isBorderBeingPressed = false;
    b_isExpandingDisabled = false;
    if(!isAtBorder(event->position().toPoint()) && b_isCursorResize)
    {
        setCursor(QCursor(Qt::ArrowCursor));
        b_isCursorResize = false;
    }

    QFrame::mouseReleaseEvent(event);
}

// *****************************************************************
// private
// *****************************************************************

int
TP_PlaylistBottomFrame::isAtBorder(QPoint I_point) const
{
    if (height() - I_point.y() <= TP_BORDER_SIZE)
    {
        return TP_BOTTOM_BORDER;
    }
    return 0;
}
