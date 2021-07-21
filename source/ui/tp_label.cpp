#include "tp_label.h"

#include <QMouseEvent>

TP_Label::TP_Label(QWidget *parent) :
    QLabel(parent)
{
    setMouseTracking(true);
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_Label::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}
