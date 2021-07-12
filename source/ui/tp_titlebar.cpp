#include "tp_titlebar.h"

#include <QMouseEvent>

TP_TitleBar::TP_TitleBar(QWidget *parent) :
    QFrame(parent)
  , pressedPosition(0, 0)
{

}

// *****************************************************************
// private
// *****************************************************************

void
TP_TitleBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isBeingPressed = true;
        pressedPosition = event->globalPosition().toPoint();
    }
    QWidget::mousePressEvent(event);
}

void
TP_TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if(isBeingPressed)
    {
        QPoint movePoint = event->globalPosition().toPoint() - pressedPosition;
        pressedPosition = event->globalPosition().toPoint();
        window()->move(window()->pos() + movePoint);
    }
    QWidget::mouseMoveEvent(event);
}

void
TP_TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    isBeingPressed = false;
    QWidget::mouseReleaseEvent(event);
}
