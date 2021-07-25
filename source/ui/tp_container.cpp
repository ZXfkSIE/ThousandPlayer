﻿#include "tp_container.h"

#include <QMouseEvent>

TP_Container::TP_Container(QWidget *parent) :
    QWidget(parent)
{
    setMouseTracking(true);
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_Container::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}