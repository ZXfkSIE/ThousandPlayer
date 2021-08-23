#include "tp_mainwindow.h"
#include "./ui_tp_mainwindow.h"

#include "tp_globalconst.h"

#include <QMouseEvent>
#include <QStyle>
#include <QWindow>

TP_MainWindow::TP_MainWindow(QWidget *parent) :
    QWidget(parent)
  , ui { new Ui::TP_MainWindow }
  , b_isBorderBeingPressed { false }
  , b_isCursorResize { false }
  , b_isExpandingDisabled { false }
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
    setIcon_Repeat();
}

TP_MainWindow::~TP_MainWindow()
{
    delete ui;
}

void
TP_MainWindow::setCurrentAudioProperties(
        QString I_qstr_Format,
        int     bitDepth,
        int     sampleRate,
        int     bitRate,
        int     duration)
{
    ui->label_Format->setText( QString(" ") + I_qstr_Format + QString(" ") );
    ui->label_BitDepth->setText(
                QString(" ") + ( ( bitDepth == -1 ) ? QString("-") : QString::number( bitDepth ) )
                + QString( " bits " )
                );
    ui->label_SampleRate->setText(
                QString(" ") + ( ( sampleRate == -1 ) ? QString("-") : QString::number( sampleRate ) )
                + QString( " KHz " )
                );
    ui->label_Bitrate->setText(
                QString(" ") + ( ( bitRate == -1 ) ? QString("-") : QString::number( bitRate ) )
                + QString( " kbps " )
                );
    ui->label_DurationTime->setText( QString("%1:%2")
                                     .arg(duration / 60, 2, 10, QLatin1Char('0') )
                                     .arg(duration % 60, 2, 10, QLatin1Char('0') )
                                     );
}

// *****************************************************************
// public slots:
// *****************************************************************

void
TP_MainWindow::slot_PlaylistWindowShown()
{
    ui->pushButton_Playlist->setStyleSheet("color: rgb(255, 255, 255);");
    b_isPlaylistWindowShown = true;
}

void
TP_MainWindow::slot_PlaylistWindowHidden()
{
    ui->pushButton_Playlist->setStyleSheet("color: rgb(0, 0, 0);");
    b_isPlaylistWindowShown = false;
}

// *****************************************************************
// private slots:
// *****************************************************************

void
TP_MainWindow::on_pushButton_Exit_clicked() const
{
    QApplication::exit();
}

void
TP_MainWindow::on_pushButton_Expand_clicked()
{
    QRect rect_ScreenGeometry = window()->windowHandle()->screen()->geometry();
    QRect rect_CurrentGeometry = geometry();

    if(width() < rect_ScreenGeometry.width())
    {
        rect_CurrentGeometry.setLeft(rect_ScreenGeometry.left());
        rect_CurrentGeometry.setRight(rect_ScreenGeometry.right());
    }
    else    // Return to the minimum width
        rect_CurrentGeometry.setRight(rect_CurrentGeometry.left() + minimumWidth());

    setGeometry( rect_CurrentGeometry );
}

void
TP_MainWindow::on_pushButton_Playlist_clicked()
{
    if(b_isPlaylistWindowShown)
        emit signal_hidePlaylistWindow();
    else
        emit signal_openPlaylistWindow();
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

void
TP_MainWindow::setIcon_Repeat()
{
    ui->pushButton_Mode->setIcon( QIcon{":/image/icon_Repeat.svg"} );
    ui->pushButton_Mode->setIconSize( QSize{ TP::iconSize_Repeat, TP::iconSize_Repeat } );
}
