#include "tp_mainwindow.h"
#include "./ui_tp_mainwindow.h"

#include "tp_globalconst.h"
#include "tp_globalfunction.h"
#include "tp_globalvariable.h"

#include "tp_menu.h"
#include "tp_timeslider.h"

#include <QAudio>
#include <QFileInfo>
#include <QListWidgetItem>
#include <QMouseEvent>
#include <QStyle>
#include <QToolTip>
#include <QWindow>

#include <filesystem>

TP_MainWindow::TP_MainWindow(QWidget *parent) :
    QWidget                 { parent }
  , ui                      { new Ui::TP_MainWindow }
  , b_isBorderBeingPressed  { false }
  , b_isCursorResize        { false }
{
    ui->setupUi(this);
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint );

    initializeConnection();
    initializeMenu();
    initializeUI();
}

TP_MainWindow::~TP_MainWindow()
{
    TP::config().setVolume( ui->slider_Volume->value() );
    delete ui;
}

void
TP_MainWindow::setPlay()
{
    setIcon_Pause();
}


void
TP_MainWindow::setPause()
{
    setIcon_Play();
}


void
TP_MainWindow::setStop()
{
    setIcon_Play();
    setAudioPropertyLabels();
}


void
TP_MainWindow::setAudioInformation( QListWidgetItem *I_item )
{
    const QUrl url = I_item->data( TP::role_URL ).toUrl();

    QString qstr_format { "N/A" };

    QString extension { TP::extension( url.toLocalFile() ) };

    if( extension == QString { "flac" } )
        qstr_format = "FLAC";
    if( extension == QString { "mp3" } )
        qstr_format = "MP3";

    setAudioPropertyLabels( qstr_format,
                            I_item->data( TP::role_BitDepth ).toInt(),
                            I_item->data( TP::role_SampleRate ).toInt(),
                            I_item->data( TP::role_Bitrate ).toInt(),
                            I_item->data( TP::role_Duration ).toInt()
                            );
}


void
TP_MainWindow::setFileNotFound()
{
    setAudioPropertyLabels();
}


void
TP_MainWindow::setCover( const QImage &I_image )
{
    if( I_image.isNull() )
        ui->label_Cover->setImage();
    else
        ui->label_Cover->setImage( QPixmap::fromImage( I_image ) );
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
TP_MainWindow::slot_updateDuration( qint64 ms )
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
TP_MainWindow::slot_volumeSliderChanged( int I_volume )
{
    // qDebug() << "[SLOT] TP_MainWindow::slot_volumeSliderChanged(" << I_volume << ")";
    ui->label_VolumeIcon->setIcon( I_volume );
    QToolTip::showText( QCursor::pos(), QString::number( I_volume ), nullptr, {}, 1500);
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
TP_MainWindow::on_pushButton_Minimize_clicked()
{
    showMinimized();
}


void
TP_MainWindow::on_pushButton_Playlist_clicked()
{
    if( b_isPlaylistWindowShown )
        emit signal_hidePlaylistWindow();
    else
        emit signal_openPlaylistWindow();
}


void
TP_MainWindow::on_pushButton_Config_clicked()
{
    emit signal_openConfigWindow();
}


void
TP_MainWindow::on_pushButton_Play_clicked()
{
    if ( TP::playbackState() == QMediaPlayer::PlayingState )
        emit signal_pauseButtonPushed();
    else
        emit signal_playButtonPushed();
}


void
TP_MainWindow::on_pushButton_Stop_clicked()
{
    emit signal_stopButtonPushed();
}


void
TP_MainWindow::on_pushButton_Next_clicked()
{
    emit signal_nextButtonPushed();
}


void
TP_MainWindow::on_pushButton_Previous_clicked()
{
    emit signal_previousButtonPushed();
}


void
TP_MainWindow::on_action_setMode_SingleTime_triggered()
{
    ui->pushButton_Mode->setIcon( QIcon{":/image/icon_SingleTime.svg"} );
    ui->pushButton_Mode->setIconSize( QSize{ TP::iconSize_SingleTime, TP::iconSize_SingleTime } );
    TP::config().setPlayMode( TP::singleTime );
    emit signal_modeIsNotShuffle();
}


void
TP_MainWindow::on_action_setMode_Repeat_triggered()
{
    ui->pushButton_Mode->setIcon( QIcon{":/image/icon_Repeat.svg"} );
    ui->pushButton_Mode->setIconSize( QSize{ TP::iconSize_Repeat, TP::iconSize_Repeat } );
    TP::config().setPlayMode( TP::repeat );
    emit signal_modeIsNotShuffle();
}


void
TP_MainWindow::on_action_setMode_Sequential_triggered()
{
    ui->pushButton_Mode->setIcon( QIcon{":/image/icon_Sequential.svg"} );
    ui->pushButton_Mode->setIconSize( QSize{ TP::iconSize_Sequential, TP::iconSize_Sequential });
    TP::config().setPlayMode( TP::sequential );
    emit signal_modeIsNotShuffle();
}


void
TP_MainWindow::on_action_setMode_Shuffle_triggered()
{
    ui->pushButton_Mode->setIcon( QIcon{":/image/icon_Shuffle.svg"} );
    ui->pushButton_Mode->setIconSize( QSize{ TP::iconSize_Shuffle, TP::iconSize_Shuffle } );
    TP::config().setPlayMode( TP::shuffle );
}


// *****************************************************************
// private override
// *****************************************************************


void
TP_MainWindow::changeEvent( QEvent *event )
{
    if( event->type() == QEvent::WindowStateChange )
    {
        if( isMinimized() )
        {
            // qDebug() << "[SIGNAL] TP_MainWindow::signal_minimizeWindow()";
            emit signal_minimizeWindow();
        }
        else
        {
            // qDebug() << "[SIGNAL] TP_MainWindow::signal_restoreWindow()";
            emit signal_restoreWindow();
        }
    }

    if( event->type() == QEvent::ActivationChange && isActiveWindow() )
    {
        // qDebug() << "[SIGNAL] TP_MainWindow::signal_activateWindow()";
        emit signal_activateWindow();
    }
}


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

    connect(ui->slider_Volume,      &QSlider::valueChanged,
            this,                   &TP_MainWindow::slot_volumeSliderChanged);
    connect(ui->label_VolumeIcon,   &TP_VolumeIcon::signal_setVolume,
            ui->slider_Volume,      &QSlider::setValue);
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
TP_MainWindow::initializeUI()
{
    show();

    ui->pushButton_Minimize ->setIcon( QIcon{ ":/image/icon_Minimize.svg" } );
    ui->pushButton_Expand   ->setIcon( QIcon{ ":/image/icon_Expand.svg" } );
    ui->pushButton_Exit     ->setIcon( QIcon{ ":/image/icon_Exit.svg" } );

    // If the volume is 0, then the valueChanged signal will not be triggered
    // since the original value is 0.
    ui->label_VolumeIcon->initialize();
    if( ! TP::config().getVolume() )
        ui->slider_Volume->setValue( 50 );
    ui->slider_Volume->setValue( TP::config().getVolume() );

    ui->label_Cover->setImage();

    ui->pushButton_Previous ->setIcon( QIcon{ ":/image/icon_Previous.svg" } );
    ui->pushButton_Stop     ->setIcon( QIcon{ ":/image/icon_Stop.svg" } );
    setIcon_Play();
    ui->pushButton_Next     ->setIcon( QIcon{ ":/image/icon_Next.svg" } );

    switch( TP::config().getPlayMode() )
    {
    case TP::singleTime :
        on_action_setMode_SingleTime_triggered();
        break;

    case TP::repeat :
        on_action_setMode_Repeat_triggered();
        break;

    case TP::sequential :
        on_action_setMode_Sequential_triggered();
        break;

    case TP::shuffle :
        on_action_setMode_Shuffle_triggered();
        break;
    }

    setAudioPropertyLabels();

    // Pending implementation
    ui->pushButton_Lyrics->hide();
}


void
TP_MainWindow::setIcon_Play()
{
    ui->pushButton_Play->setIcon( QIcon{ ":/image/icon_Play.svg" } );
    ui->pushButton_Play->setIconSize( QSize( TP::iconSize_Play, TP::iconSize_Play ) );
}


void
TP_MainWindow::setIcon_Pause()
{
    ui->pushButton_Play->setIcon( QIcon{ ":/image/icon_Pause.svg" } );
    ui->pushButton_Play->setIconSize( QSize( TP::iconSize_Pause, TP::iconSize_Pause ) );
}


void
TP_MainWindow::setAudioPropertyLabels(
        const QString & I_qstr_Format,
        int             bitDepth,
        int             sampleRate,
        int             bitRate,
        int             duration )
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
TP_MainWindow::isAtBorder( const QPoint &I_point ) const
{
    if ( I_point.x() <= TP::borderSize )
    {
        return TP::leftBorder;
    }
    else if ( width() - I_point.x() <= TP::borderSize )
    {
        return TP::rightBorder;
    }
    return TP::notAtBorder;
}


QString
TP_MainWindow::convertTime(qint64 second) const
{
    return QString( "%1:%2" )
            .arg( second / 60, 2, 10, QLatin1Char('0') )
            .arg( second % 60, 2, 10, QLatin1Char('0') );
}
