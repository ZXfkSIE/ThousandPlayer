#include "tp_mainwindow.h"
#include "./ui_tp_mainwindow.h"

#include "tp_globalconst.h"

#include <QMouseEvent>

TP_MainWindow::TP_MainWindow(QWidget *parent) :
    QWidget(parent)
  , ui(new Ui::TP_MainWindow)
  , b_isBorderBeingPressed(false)
  , b_isCursorResize(false)
  , b_isExpandingDisabled(false)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    ui->pushButton_Minimize->setIcon( QIcon{":/image/icon_Minimize.svg"} );
    ui->pushButton_Expand->setIcon( QIcon{":/image/icon_Expand.svg"} );
    ui->pushButton_Exit->setIcon( QIcon{":/image/icon_Exit.svg"} );

    ui->label_VolumeIcon->initialize();
    ui->label_VolumeIcon->setIcon(true);

    ui->widget_VisualContainer->initialize();
    ui->widget_VisualContainer->switchWidget(TP::albumCover);

    ui->pushButton_Previous->setIcon( QIcon{":/image/icon_Previous.svg"} );
    ui->pushButton_Stop->setIcon( QIcon{":/image/icon_Stop.svg"} );
    ui->pushButton_Play->setIcon( QIcon{":/image/icon_Play.svg"} );
    ui->pushButton_Next->setIcon( QIcon{":/image/icon_Next.svg"} );
    ui->pushButton_Mode->setIcon( QIcon{":/image/icon_Repeat.svg"} );
}

TP_MainWindow::~TP_MainWindow()
{
    delete ui;
}

// *****************************************************************
// private slots:
// *****************************************************************

void
TP_MainWindow::on_pushButton_Exit_clicked() const
{
    QApplication::exit();
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_MainWindow::mousePressEvent(QMouseEvent *event)
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
TP_MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint eventPosition = event->position().toPoint();
    // qDebug()<< "Main Window cursor position: " << eventPosition;

    if (b_isBorderBeingPressed)
    {
        int differenceX = event->globalPosition().toPoint().x() - pressedGlobalPosition.x();
        QRect rect_Geometry = geometry();

        switch (cursorPositionType)
        {
        case TP::leftBorder:
            if (differenceX < 0 && b_isExpandingDisabled)
                return;

            rect_Geometry.setLeft(rect_Geometry.left() + differenceX);
            if(rect_Geometry.width() >= minimumWidth())
                setGeometry(rect_Geometry);
            pressedGlobalPosition = event->globalPosition().toPoint();
            if (event->position().toPoint().x() > width() - minimumWidth() + TP::borderSize)
                b_isExpandingDisabled = true;

            break;              // case TP_LEFT_BORDER

        case TP::rightBorder:
            if (differenceX > 0 && b_isExpandingDisabled)
                return;

            rect_Geometry.setRight(rect_Geometry.right() + differenceX);
            setGeometry(rect_Geometry);
            pressedGlobalPosition = event->globalPosition().toPoint();
            if (event->position().toPoint().x() < width() - TP::borderSize)
                b_isExpandingDisabled = true;

            break;              // case TP_RIGHT_BORDER

        default:
            break;
        }
    }
    else                        // if (b_isBorderBeingPressed)
    {
        cursorPositionType = isAtBorder(eventPosition);
        switch (cursorPositionType)
        {
        case TP::notAtBorder:
            if (b_isCursorResize)
            {
                setCursor(QCursor(Qt::ArrowCursor));
                b_isCursorResize = false;
            }
            break;

        case TP::leftBorder:
        case TP::rightBorder:
            if (! b_isCursorResize)
            {
                setCursor(QCursor(Qt::SizeHorCursor));
                b_isCursorResize = true;
            }
            break;

        default:
            break;
        }
    }

    QWidget::mouseMoveEvent(event);
}

void
TP_MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    b_isBorderBeingPressed = false;
    b_isExpandingDisabled = false;
    if(!isAtBorder(event->position().toPoint()) && b_isCursorResize)
    {
        setCursor(QCursor(Qt::ArrowCursor));
        b_isCursorResize = false;
    }

    QWidget::mouseReleaseEvent(event);
}

// *****************************************************************
// private
// *****************************************************************

TP::CursorPositionType
TP_MainWindow::isAtBorder(QPoint I_point) const
{
    if (I_point.x() <= TP::borderSize)
    {
        return TP::leftBorder;
    }
    else if (width() - I_point.x() <= TP::borderSize)
    {
        return TP::rightBorder;
    }
    return TP::notAtBorder;
}
