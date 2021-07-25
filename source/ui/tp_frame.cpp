#include "tp_frame.h"

#include <QMouseEvent>

TP_Frame::TP_Frame(QWidget *parent) :
    QFrame(parent)
{
    setMouseTracking(true);
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_Frame::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}
