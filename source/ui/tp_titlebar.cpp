#include "tp_titlebar.h"

#include <QMouseEvent>

TP_TitleBar::TP_TitleBar(QWidget *parent) :
    QFrame(parent),
    currentPosition(0, 0)
{

}

// 鼠标相对于窗体的位置 event->globalPos() - this->pos()
void
TP_TitleBar::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        isBeingPressed = true;
        currentPosition = event->pos();
    }
}

// 若鼠标左键被按下，移动窗体位置
void
TP_TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if(isBeingPressed)
        window()->move(event->pos() - currentPosition + pos());
}

// 设置鼠标未被按下
void
TP_TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    isBeingPressed = false;
}
