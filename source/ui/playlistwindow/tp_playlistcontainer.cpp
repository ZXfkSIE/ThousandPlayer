#include "tp_playlistcontainer.h"

#include "tp_globalconst.h"

#include <QMouseEvent>

TP_PlaylistContainer::TP_PlaylistContainer(QWidget *parent) :
    QWidget(parent)
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
TP_PlaylistContainer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && cursorPositionType)
    {
        b_isBorderBeingPressed = true;
        b_isExpandingDisabled = false;
        pressedGlobalPosition = event->globalPosition().toPoint();
    }

    QWidget::mousePressEvent(event);
}

void
TP_PlaylistContainer::mouseMoveEvent(QMouseEvent *event)
{
    QPoint eventPosition = event->position().toPoint();

    if (b_isBorderBeingPressed)
    {
        int differenceX = event->globalPosition().toPoint().x() - pressedGlobalPosition.x();
        QRect rect_Geometry = window()->geometry();

        switch (cursorPositionType)
        {
        case TP_LEFT_BORDER:
            if (differenceX < 0 && b_isExpandingDisabled)
                return;

            rect_Geometry.setLeft(rect_Geometry.left() + differenceX);
            if(rect_Geometry.width() >= window()->minimumWidth())
                window()->setGeometry(rect_Geometry);
            pressedGlobalPosition = event->globalPosition().toPoint();
            if (event->position().toPoint().x() > width() - window()->minimumWidth() + TP_BORDER_SIZE)
                b_isExpandingDisabled = true;

            break;              // case TP_LEFT_BORDER

        case TP_RIGHT_BORDER:
            if (differenceX > 0 && b_isExpandingDisabled)
                return;

            rect_Geometry.setRight(rect_Geometry.right() + differenceX);
            window()->setGeometry(rect_Geometry);
            pressedGlobalPosition = event->globalPosition().toPoint();
            if (event->position().toPoint().x() < width() -  TP_BORDER_SIZE)
                b_isExpandingDisabled = true;

            break;              // case TP_RIGHT_BORDER
        }
    }
    else                        // if (b_isBorderBeingPressed)
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

        case TP_LEFT_BORDER:
        case TP_RIGHT_BORDER:
            if (! b_isCursorResize)
            {
                setCursor(QCursor(Qt::SizeHorCursor));
                b_isCursorResize = true;
            }
            break;
        }
    }                           // if (b_isBorderBeingPressed)

    QWidget::mouseMoveEvent(event);
}

void
TP_PlaylistContainer::mouseReleaseEvent(QMouseEvent *event)
{
    b_isBorderBeingPressed = false;
    b_isExpandingDisabled = false;

    QWidget::mouseReleaseEvent(event);
}

// *****************************************************************
// private
// *****************************************************************

int
TP_PlaylistContainer::isAtBorder(QPoint I_point) const
{
    if (I_point.x() <= TP_BORDER_SIZE)
    {
        return TP_LEFT_BORDER;
    }
    else if (width() - I_point.x() <= TP_BORDER_SIZE)
    {
        return TP_RIGHT_BORDER;
    }
    return 0;
}
