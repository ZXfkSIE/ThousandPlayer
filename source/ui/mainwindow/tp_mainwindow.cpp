#include "tp_mainwindow.h"
#include "./ui_tp_mainwindow.h"

#include "tp_globalconst.h"

#include "tp_menu.h"
#include "tp_timeslider.h"

#include <QAudio>
#include <QListWidgetItem>
#include <QMouseEvent>
#include <QStyle>
#include <QToolTip>
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
  , b_isPlaying { false }
{
    ui->setupUi(this);
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint );

    initializeConnection();
    initializeMenu();

    ui->pushButton_Minimize->setIcon( QIcon{":/image/icon_Minimize.svg"} );
    ui->pushButton_Expand->setIcon( QIcon{":/image/icon_Expand.svg"} );
    ui->pushButton_Exit->setIcon( QIcon{":/image/icon_Exit.svg"} );

    ui->label_VolumeIcon->initialize();

    ui->widget_VisualContainer->initialize();
    ui->widget_VisualContainer->switchWidget( TP::albumCover );

    ui->pushButton_Previous->setIcon( QIcon{":/image/icon_Previous.svg"} );
    ui->pushButton_Stop->setIcon( QIcon{":/image/icon_Stop.svg"} );
    setIcon_Play();
    ui->pushButton_Next->setIcon( QIcon{":/image/icon_Next.svg"} );
    on_action_setMode_SingleTime_triggered();

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

void
TP_MainWindow::setPause()
{
    b_isPlaying = false;
    setIcon_Play();
}

void
TP_MainWindow::setStop()
{
    b_isPlaying = false;
    setIcon_Play();
    setAudioPropertyLabels();
}

void
TP_MainWindow::setAudioInformation(const QListWidgetItem &I_listWidgetItem)
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

void
TP_MainWindow::setFileNotFound()
{
    setAudioPropertyLabels();
}

void
TP_MainWindow::setVolumeSliderValue(int value)
{
    ui->slider_Volume->setValue( value );
}

// *****************************************************************
// public slots:
// *****************************************************************

void
TP_MainWindow::slot_changeVolumeSlider(float linearVolume)
{
    float logarithmicVolume = QAudio::convertVolume(linearVolume,
                                                    QAudio::LinearVolumeScale,
                                                    QAudio::LogarithmicVolumeScale);

    ui->slider_Volume->setValue( std::round(logarithmicVolume * 100) );
}

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
TP_MainWindow::slot_updateDuration(qint64 ms)
{
    qint64 second = ms / 1000;                       // convert ms to s
    if ( !ui->slider_Time->isSliderDown() )
        ui->slider_Time->setValue( second );

    // DO NOT update time label here.
    // Let slot_timeSliderChanged do it.
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
TP_MainWindow::slot_leftButtonReleased()
{
    emit signal_leftButtonReleased();
}

void
TP_MainWindow::slot_timeSliderChanged( int second )
{
    ui->label_CurrentTime->setText( convertTime( second ) );
}

void
TP_MainWindow::slot_timeSliderPressed( int second )
{
    emit signal_timeSliderPressed( second );
}

void
TP_MainWindow::slot_volumeSliderChanged( int volume )
{
    ui->label_VolumeIcon->setIcon( volume );
    QToolTip::showText( QCursor::pos(), QString::number(volume), nullptr, {}, 1500);
    emit signal_volumeSliderValueChanged(
                QAudio::convertVolume(ui->slider_Volume->value() / 100.0,
                                      QAudio::LogarithmicVolumeScale,
                                      QAudio::LinearVolumeScale)
                );
}

void
TP_MainWindow::on_pushButton_Exit_clicked() const
{
    QApplication::exit();
}

void
TP_MainWindow::on_pushButton_Expand_clicked()
{
    QRect ScreenGeometry = window()->windowHandle()->screen()->geometry();
    QRect CurrentGeometry = geometry();

    if( width() < ScreenGeometry.width() )
    {
        CurrentGeometry.setLeft( ScreenGeometry.left() );
        CurrentGeometry.setRight( ScreenGeometry.right() );
    }
    else    // Return to the minimum width
        CurrentGeometry.setRight( CurrentGeometry.left() + minimumWidth() - 1 );

    setGeometry( CurrentGeometry );
}

void
TP_MainWindow::on_pushButton_Playlist_clicked()
{
    if(b_isPlaylistWindowShown)
        emit signal_hidePlaylistWindow();
    else
        emit signal_openPlaylistWindow();
}

void TP_MainWindow::on_pushButton_Play_clicked()
{
    if ( b_isPlaying )
        emit signal_pauseButtonPushed();
    else
        emit signal_playButtonPushed();
}

void TP_MainWindow::on_pushButton_Stop_clicked()
{
    emit signal_stopButtonPushed();
}

void
TP_MainWindow::on_action_setMode_SingleTime_triggered()
{
    ui->pushButton_Mode->setIcon( QIcon{":/image/icon_SingleTime.svg"} );
    ui->pushButton_Mode->setIconSize( QSize{ TP::iconSize_SingleTime, TP::iconSize_SingleTime } );
    emit signal_setMode_SingleTime();
}

void
TP_MainWindow::on_action_setMode_Repeat_triggered()
{
    ui->pushButton_Mode->setIcon( QIcon{":/image/icon_Repeat.svg"} );
    ui->pushButton_Mode->setIconSize( QSize{ TP::iconSize_Repeat, TP::iconSize_Repeat } );
    emit signal_setMode_Repeat();
}

void
TP_MainWindow::on_action_setMode_Sequential_triggered()
{
    ui->pushButton_Mode->setIcon( QIcon{":/image/icon_Sequential.svg"} );
    ui->pushButton_Mode->setIconSize( QSize{ TP::iconSize_Sequential, TP::iconSize_Sequential });
    emit signal_setMode_Sequential();
}

void
TP_MainWindow::on_action_setMode_Shuffle_triggered()
{
    ui->pushButton_Mode->setIcon( QIcon{":/image/icon_Shuffle.svg"} );
    ui->pushButton_Mode->setIconSize( QSize{ TP::iconSize_Shuffle, TP::iconSize_Shuffle } );
    emit signal_setMode_Shuffle();
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_MainWindow::mousePressEvent( QMouseEvent *event )
{
    if ( event->button() == Qt::LeftButton && cursorPositionType )
        b_isBorderBeingPressed = true;

    QWidget::mousePressEvent( event );
}

void
TP_MainWindow::mouseMoveEvent( QMouseEvent *event )
{
    QPoint eventPosition = event->position().toPoint();
    // qDebug()<< "Main Window cursor position: " << eventPosition;

    if ( b_isBorderBeingPressed )
    {
        QRect newGeometry = geometry();

        switch ( cursorPositionType )
        {
        case TP::leftBorder:

            newGeometry.setLeft( event->globalPosition().toPoint().x() );
            if ( newGeometry.width() < minimumWidth() )
                newGeometry.setLeft( newGeometry.right() - minimumWidth() + 1 );

            if( newGeometry != geometry() )
                emit signal_resizeWindow( this, newGeometry, TP::atLeft );

            break;              // case TP_LEFT_BORDER

        case TP::rightBorder:

            newGeometry.setRight( event->globalPosition().toPoint().x() );
            if ( newGeometry.width() < minimumWidth() )
                newGeometry.setWidth( minimumWidth() );

            if( newGeometry != geometry() )
                emit signal_resizeWindow( this, newGeometry, TP::atRight );

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
    if( b_isBorderBeingPressed )
    {
        b_isBorderBeingPressed = false;
        if( !isAtBorder(event->position().toPoint()) && b_isCursorResize )
        {
            setCursor( QCursor(Qt::ArrowCursor) );
            b_isCursorResize = false;
        }
        emit signal_leftButtonReleased();
    }

    QWidget::mouseReleaseEvent(event);
}

// *****************************************************************
// private
// *****************************************************************

void
TP_MainWindow::initializeConnection()
{
    connect(ui->frame_Title,    &TP_TitleBar::signal_moveTitleBar,
            this,               &TP_MainWindow::slot_moveTitleBar);
    connect(ui->frame_Title,    &TP_TitleBar::signal_leftButtonReleased,
            this,               &TP_MainWindow::slot_leftButtonReleased);

    connect(ui->slider_Time,    &TP_TimeSlider::valueChanged,
            this,               &TP_MainWindow::slot_timeSliderChanged);
    connect(ui->slider_Time,    &TP_TimeSlider::signal_mouseReleased,
            this,               &TP_MainWindow::slot_timeSliderPressed);

    connect(ui->slider_Volume,  &TP_VolumeSlider::valueChanged,
            this,               &TP_MainWindow::slot_volumeSliderChanged);
}

void
TP_MainWindow::initializeMenu()
{
    // Initialize menu of playback mode button
    ui->action_setMode_SingleTime->setIcon( QIcon{":/image/icon_SingleTime.svg"} );
    ui->action_setMode_Repeat->setIcon( QIcon{":/image/icon_Repeat.svg"} );
    ui->action_setMode_Sequential->setIcon( QIcon{":/image/icon_Sequential.svg"} );
    ui->action_setMode_Shuffle->setIcon( QIcon{":/image/icon_Shuffle.svg"} );

    menu_Mode = new TP_Menu { ui->pushButton_Mode };

    menu_Mode->addAction( ui->action_setMode_SingleTime );
    menu_Mode->addAction( ui->action_setMode_Repeat );
    menu_Mode->addAction( ui->action_setMode_Sequential );
    menu_Mode->addAction( ui->action_setMode_Shuffle );

    ui->pushButton_Mode->setMenu( menu_Mode );
}

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

    ui->slider_Time->setRange( 0, duration );
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
