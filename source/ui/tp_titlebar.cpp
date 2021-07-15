#include "tp_titlebar.h"

#include <QMouseEvent>

TP_TitleBar::TP_TitleBar(QWidget *parent) :
    QFrame(parent)
{

}

// *****************************************************************
// private override
// *****************************************************************

void
TP_TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        pressedRelativePosition = event->position().toPoint();

    QWidget::mousePressEvent(event);
}

void
TP_TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    window()->move(event->globalPosition().toPoint() - pressedRelativePosition);

    QWidget::mouseMoveEvent(event);
}
