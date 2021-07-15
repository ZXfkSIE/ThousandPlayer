#include "tp_playlist.h"
#include "ui_tp_playlist.h"

#include <QMouseEvent>

TP_Playlist::TP_Playlist(QWidget *parent) :
    QWidget(parent)
  , ui(new Ui::TP_Playlist)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setMouseTracking(true);

    ui->pushButton_Close->setIcon(QIcon{":/image/icon_Exit.svg"});
}

TP_Playlist::~TP_Playlist()
{
    delete ui;
}

// *****************************************************************
// private slots:
// *****************************************************************

void TP_Playlist::on_pushButton_Close_clicked()
{
    hide();
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_Playlist::mousePressEvent(QMouseEvent *event)
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
TP_Playlist::mouseMoveEvent(QMouseEvent *event)
{
    QPoint eventPosition = event->position().toPoint();
    if(eventPosition.y() < 36)
    {
        if(b_isCursorResize)
        {
            setCursor(QCursor(Qt::ArrowCursor));
            b_isCursorResize = false;
        }
        return;
    }

    if (b_isBorderBeingPressed)
    {
        int differenceX = event->globalPosition().toPoint().x() - pressedGlobalPosition.x();
        QRect rect_Geometry = geometry();

        switch (cursorPositionType)
        {
        case TP_LEFT_BORDER:
            if (differenceX < 0 && b_isExpandingDisabled)
                break;

            rect_Geometry.setLeft(rect_Geometry.left() + differenceX);
            if(rect_Geometry.width() >= minimumWidth())
                setGeometry(rect_Geometry);
            pressedGlobalPosition = event->globalPosition().toPoint();
            if (event->position().toPoint().x() > width() - minimumWidth() + 5 )
                b_isExpandingDisabled = true;

            break;              //case TP_LEFT_BORDER

        case TP_RIGHT_BORDER:
            if (differenceX > 0 && b_isExpandingDisabled)
                break;

            rect_Geometry.setRight(rect_Geometry.right() + differenceX);
            setGeometry(rect_Geometry);
            pressedGlobalPosition = event->globalPosition().toPoint();
            if (event->position().toPoint().x() < width() -  5)
                b_isExpandingDisabled = true;

            break;              //case TP_RIGHT_BORDER
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

        case TP_LEFT_BORDER:
        case TP_RIGHT_BORDER:
            if (! b_isCursorResize)
            {
                setCursor(QCursor(Qt::SizeHorCursor));
                b_isCursorResize = true;
            }

            break;
        }
    }

    QWidget::mouseMoveEvent(event);
}

void
TP_Playlist::mouseReleaseEvent(QMouseEvent *event)
{
    b_isBorderBeingPressed = false;
    b_isExpandingDisabled = false;

    QWidget::mouseReleaseEvent(event);
}

// *****************************************************************
// private
// *****************************************************************

int TP_Playlist::isAtBorder(QPoint I_qpt)
{
    if (I_qpt.x() <= TP_BORDER_SIZE)
    {
        return TP_LEFT_BORDER;
    }
    else if (width() - I_qpt.x() <= TP_BORDER_SIZE)
    {
        return TP_RIGHT_BORDER;
    }
    return 0;
}
