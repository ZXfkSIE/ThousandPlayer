#include "tp_mainwindow.h"
#include "./ui_tp_mainwindow.h"

#include "tp_globalconst.h"

#include <QListWidgetItem>
#include <QMouseEvent>
#include <QStyle>
#include <QWindow>

#include <filesystem>

// Headers of TagLib
#include <fileref.h>
#include <tag.h>

#include <flacproperties.h>

TP_MainWindow::TP_MainWindow(QWidget *parent) :
    QWidget(parent)
  , ui { new Ui::TP_MainWindow }
  , b_isBorderBeingPressed { false }
  , b_isCursorResize { false }
  , b_isExpandingDisabled { false }
  , b_isPlaying { false }
{
    ui->setupUi(this);
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint );

    connect(ui->frame_Title,    &TP_TitleBar::signal_moveTitleBar,
            this,               &TP_MainWindow::slot_moveTitleBar);

    ui->pushButton_Minimize->setIcon( QIcon{":/image/icon_Minimize.svg"} );
    ui->pushButton_Expand->setIcon( QIcon{":/image/icon_Expand.svg"} );
    ui->pushButton_Exit->setIcon( QIcon{":/image/icon_Exit.svg"} );

    ui->label_VolumeIcon->initialize();
    ui->label_VolumeIcon->setIcon(true);

    ui->widget_VisualContainer->initialize();
    ui->widget_VisualContainer->switchWidget( TP::albumCover );

    ui->pushButton_Previous->setIcon( QIcon{":/image/icon_Previous.svg"} );
    ui->pushButton_Stop->setIcon( QIcon{":/image/icon_Stop.svg"} );
    setIcon_Play();
    ui->pushButton_Next->setIcon( QIcon{":/image/icon_Next.svg"} );
    setIcon_Repeat();

    setAudioPropertyLabels();
}

TP_MainWindow::~TP_MainWindow()
{
    delete ui;
}

void
TP_MainWindow::setPlay()
{
    b_isPlaying = true;
    setIcon_Pause();
}

void TP_MainWindow::setPause()
{
    b_isPlaying = false;
    setIcon_Play();
}

void TP_MainWindow::setStop()
{
    b_isPlaying = false;
    setIcon_Play();
    setAudioPropertyLabels();
}

void TP_MainWindow::setAudioInformation(const QListWidgetItem &I_listWidgetItem)
{
    QUrl url { I_listWidgetItem.data( TP::role_URL ).value<QUrl>() };

    if( std::filesystem::exists( url.toLocalFile().toLocal8Bit().constData() ) )
    {
        TagLib::FileRef fileRef { url.toLocalFile().toLocal8Bit().constData() };

        // Set audio property labels
        QString qstr_Format;
        int bitRate = fileRef.audioProperties()->bitrate();
        int sampleRate = fileRef.audioProperties()->sampleRate() / 1000;
        int bitDepth = -1;
        int duration = fileRef.audioProperties()->lengthInSeconds();

        TP::FileFormat format { I_listWidgetItem.data(TP::role_FileType).value<TP::FileFormat>() };
        switch (format)
        {
        case TP::FileFormat::FLAC :
            qstr_Format = QString( "FLAC" );
            bitDepth = dynamic_cast<TagLib::FLAC::Properties *>( fileRef.audioProperties() )->bitsPerSample();
            break;
        case TP::FileFormat::MP3 :
            qstr_Format = QString( "MP3" );
            break;
        }

        setAudioPropertyLabels( qstr_Format, bitDepth, sampleRate, bitRate, duration );
    }
    else
        return;
}

void TP_MainWindow::setFileNotFound()
{
    setAudioPropertyLabels();
}

// *****************************************************************
// public slots:
// *****************************************************************

void
TP_MainWindow::slot_playlistWindowShown()
{
    ui->pushButton_Playlist->setStyleSheet("color: rgb(255, 255, 255);");
    b_isPlaylistWindowShown = true;
}

void
TP_MainWindow::slot_playlistWindowHidden()
{
    ui->pushButton_Playlist->setStyleSheet("color: rgb(0, 0, 0);");
    b_isPlaylistWindowShown = false;
}

void
TP_MainWindow::slot_updateDuration(qint64 I_progress)
{
    // qDebug() << QString("[SLOT] slot_updateDuration(%1)").arg(I_progress);
    I_progress /= 1000;                                 // convert ms to s
    if ( !ui->slider_Time->isSliderDown() )
        ui->slider_Time->setValue( I_progress );
    ui->label_CurrentTime->setText( convertTime(I_progress) );
}

// *****************************************************************
// private slots:
// *****************************************************************

void
TP_MainWindow::slot_moveTitleBar( QRect newGeometry )
{
    emit signal_moveWindow( this, newGeometry );
}

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

void TP_MainWindow::on_pushButton_Stop_clicked()
{
    emit signal_stopButtonPushed();
}

void TP_MainWindow::on_pushButton_Play_clicked()
{
    if ( b_isPlaying )
        emit signal_pauseButtonPushed();
    else
        emit signal_playButtonPushed();
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
        switch ( cursorPositionType )
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
        setCursor( QCursor(Qt::ArrowCursor) );
        b_isCursorResize = false;
    }

    QWidget::mouseReleaseEvent(event);
}

// *****************************************************************
// private
// *****************************************************************

void
TP_MainWindow::setIcon_Play()
{
    ui->pushButton_Play->setIcon( QIcon{":/image/icon_Play.svg"} );
    ui->pushButton_Play->setIconSize( QSize( TP::iconSize_Play, TP::iconSize_Play ) );
}

void
TP_MainWindow::setIcon_Pause()
{
    ui->pushButton_Play->setIcon( QIcon{":/image/icon_Pause.svg"} );
    ui->pushButton_Play->setIconSize( QSize( TP::iconSize_Pause, TP::iconSize_Pause ) );
}

void
TP_MainWindow::setIcon_Repeat()
{
    ui->pushButton_Mode->setIcon( QIcon{":/image/icon_Repeat.svg"} );
    ui->pushButton_Mode->setIconSize( QSize{ TP::iconSize_Repeat, TP::iconSize_Repeat } );
}

void
TP_MainWindow::setAudioPropertyLabels(
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
    ui->label_DurationTime->setText( convertTime( duration ) );

    ui->slider_Time->setMaximum( duration );
}

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

QString
TP_MainWindow::convertTime(qint64 second) const
{
    return QString("%1:%2")
            .arg(second / 60, 2, 10, QLatin1Char('0') )
            .arg(second % 60, 2, 10, QLatin1Char('0') );
}
