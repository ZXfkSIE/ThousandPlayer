#include "tp_button.h"

#include <QMouseEvent>

TP_Button::TP_Button(QWidget *parent) :
    QPushButton(parent)
{
    setMouseTracking(true);
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_Button::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}
