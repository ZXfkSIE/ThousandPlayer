﻿#include "tp_titlebar.h"

#include <QMouseEvent>

TP_TitleBar::TP_TitleBar(QWidget *parent) :
    QFrame { parent }
  , b_isBeingPressed { false }
{
    setMouseTracking(true);
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        b_isBeingPressed = true;
        pressedRelativePosition = event->position().toPoint();
    }

    QFrame::mousePressEvent(event);
}

void
TP_TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if(b_isBeingPressed)
    {
        emit signal_moveWindow( window(), event->globalPosition().toPoint() - pressedRelativePosition );
        QFrame::mouseMoveEvent(event);
    }
    else
        event->ignore();
}

void
TP_TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    b_isBeingPressed = false;
    QWidget::mouseReleaseEvent(event);
}
