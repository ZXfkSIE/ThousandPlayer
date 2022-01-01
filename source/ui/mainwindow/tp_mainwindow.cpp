#include "tp_mainwindow.h"
#include "./ui_tp_mainwindow.h"

#include "tp_cmakeconfig.h"
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

TP_MainWindow::TP_MainWindow( QWidget *parent ) :
    QWidget                 { parent }
  , ui                      { new Ui::TP_MainWindow }
  , b_isBorderBeingPressed  { false }
  , b_isCursorResize        { false }
  , trayIcon                { new QSystemTrayIcon { this } }
{
    ui->setupUi( this );
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
TP_MainWindow::initializeVolume()
{
    // If the volume is 0, then the valueChanged signal will not be triggered
    // since the original value is 0.
    if( ! TP::config().getVolume() )
        ui->slider_Volume->setValue( 50 );
    ui->slider_Volume->setValue( TP::config().getVolume() );
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
    setAudioInformation();
}


void
TP_MainWindow::updatePosition( qint64 I_ms )
{
    if ( ! ui->slider_Time->isSliderDown() )
        ui->slider_Time->setValue( I_ms / 1000 );     // convert ms to s

    // DO NOT update time label here.
    // Let on_slider_Time_valueChanged() do it.
}


void
TP_MainWindow::setAudioInformation( const QListWidgetItem *I_item )
{
    if( I_item )
    {
        const auto &url { I_item->data( TP::role_URL ).toUrl() };

        QString extension { TP::extension( url.toLocalFile() ) };

        setAudioPropertyLabels( extension.toUpper(),
                                I_item->data( TP::role_BitDepth ).toInt(),
                                I_item->data( TP::role_SampleRate ).toInt(),
                                I_item->data( TP::role_Bitrate ).toInt(),
                                I_item->data( TP::role_Duration ).toInt(),
                                TP::getReplayGainFromItem( I_item )
                                );
    }
    else
        setAudioPropertyLabels();

    setAudioInfoLabel( I_item );
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
    ui->pushButton_Playlist->setStyleSheet( "color: rgb(255, 255, 255);" );
    b_isPlaylistWindowShown = true;
}


void
TP_MainWindow::slot_playlistWindowHidden()
{
    ui->pushButton_Playlist->setStyleSheet( "color: rgb(0, 0, 0);" );
    b_isPlaylistWindowShown = false;
}


void
TP_MainWindow::slot_lyricsWindowShown()
{
    ui->pushButton_Lyrics->setStyleSheet( "color: rgb(255, 255, 255);" );
    b_isLyricsWindowShown = true;
}


void
TP_MainWindow::slot_lyricsWindowHidden()
{
    ui->pushButton_Lyrics->setStyleSheet( "color: rgb(0, 0, 0);" );
    b_isLyricsWindowShown = false;
}


void
TP_MainWindow::slot_changeFontOfAudioInfoLabel()
{
    ui->label_AudioInfo->setFont( TP::config().getAudioInfoLabelFont() );
}

// *****************************************************************
// private slots:
// *****************************************************************

void
TP_MainWindow::slot_trayIcon_activated( QSystemTrayIcon::ActivationReason I_reason )
{
    // The DoubleClick signal cannot be triggered under X11 and macOS.
    // See https://doc.qt.io/qt-6/qsystemtrayicon.html#ActivationReason-enum
    // and search "enum QSystemTrayIcon::ActivationReason".
#ifdef Q_OS_WINDOWS
    if( I_reason == QSystemTrayIcon::DoubleClick )
#else
    if( I_reason == QSystemTrayIcon::Trigger )
#endif
        on_action_trayIcon_Restore_triggered();
}

void
TP_MainWindow::on_action_trayIcon_Restore_triggered()
{
    show();
    trayIcon->hide();
    emit signal_restoreWindow();
}


void
TP_MainWindow::on_action_trayIcon_Exit_triggered()
{
    QApplication::exit();
}


void
TP_MainWindow::slot_titleBarMoved( const QRect &I_geometry )
{
    emit signal_moveWindow( this, I_geometry );
}


void
TP_MainWindow::slot_leftButtonReleased()
{
    emit signal_windowChanged();
}


void
TP_MainWindow::on_slider_Time_valueChanged( int I_second )
{
    ui->label_CurrentTime->setText( convertTime( I_second ) );
}


void
TP_MainWindow::slot_timeSliderPressed( int I_second )
{
    emit signal_timeSliderPressed( I_second );
}


void
TP_MainWindow::on_slider_Volume_valueChanged( int I_volume )
{
    // qDebug() << "[TP_MainWindow] slot_volumeSliderChanged(" << I_volume << ")";
    ui->label_VolumeIcon->setIcon( I_volume );
    QToolTip::showText( QCursor::pos(), QString::number( I_volume ), nullptr, {}, 1500);
    emit signal_volumeSliderValueChanged(
                QAudio::convertVolume(ui->slider_Volume->value() / 100.0,
                                      QAudio::LogarithmicVolumeScale,
                                      QAudio::LinearVolumeScale)
                );
}


void
TP_MainWindow::on_pushButton_Exit_clicked()
{
    if( TP::config().isTrayIconEnabled() )
    {
        hide();
        trayIcon->show();
        emit signal_minimizeWindow();
    }
    else
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
    emit signal_windowChanged();
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


void TP_MainWindow::on_pushButton_Lyrics_clicked()
{
    if( b_isLyricsWindowShown )
        emit signal_hideLyricsWindow();
    else
        emit signal_openLyricsWindow();
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
    ui->pushButton_Mode->setIcon( QIcon{ ":/image/icon_SingleTime.svg" } );
    ui->pushButton_Mode->setIconSize( QSize{ TP::iconSize_SingleTime, TP::iconSize_SingleTime } );
    ui->pushButton_Mode->setToolTip( tr( "Single time mode" ) );
    TP::config().setPlayMode( TP::singleTime );
    emit signal_modeIsNotShuffle();
}


void
TP_MainWindow::on_action_setMode_Repeat_triggered()
{
    ui->pushButton_Mode->setIcon( QIcon{ ":/image/icon_Repeat.svg" } );
    ui->pushButton_Mode->setIconSize( QSize{ TP::iconSize_Repeat, TP::iconSize_Repeat } );
    ui->pushButton_Mode->setToolTip( tr( "Repeat mode" ) );
    TP::config().setPlayMode( TP::repeat );
    emit signal_modeIsNotShuffle();
}


void
TP_MainWindow::on_action_setMode_Sequential_triggered()
{
    ui->pushButton_Mode->setIcon( QIcon{ ":/image/icon_Sequential.svg" } );
    ui->pushButton_Mode->setIconSize( QSize{ TP::iconSize_Sequential, TP::iconSize_Sequential });
    ui->pushButton_Mode->setToolTip( tr( "Sequential mode" ) );
    TP::config().setPlayMode( TP::sequential );
    emit signal_modeIsNotShuffle();
}


void
TP_MainWindow::on_action_setMode_Shuffle_triggered()
{
    ui->pushButton_Mode->setIcon( QIcon{ ":/image/icon_Shuffle.svg" } );
    ui->pushButton_Mode->setIconSize( QSize{ TP::iconSize_Shuffle, TP::iconSize_Shuffle } );
    ui->pushButton_Mode->setToolTip( tr( "Shuffle mode" ) );
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
    if ( event->button() == Qt::LeftButton
         && cursorPositionType != TP::notAtBorder )
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
        auto newGeometry = geometry();

        switch ( cursorPositionType )
        {
        case TP::leftBorder:

            newGeometry.setLeft( event->globalPosition().toPoint().x() );
            if ( newGeometry.width() < minimumWidth() )
                newGeometry.setLeft( newGeometry.right() - minimumWidth() + 1 );

            if( newGeometry != window()->geometry() )
                emit signal_resizeWindow( this, newGeometry, TP::atLeft );

            break;              // case TP_LEFT_BORDER

        case TP::rightBorder:

            newGeometry.setRight( event->globalPosition().toPoint().x() );
            if ( newGeometry.width() < minimumWidth() )
                newGeometry.setWidth( minimumWidth() );

            if( newGeometry != window()->geometry() )
                emit signal_resizeWindow( this, newGeometry, TP::atRight );

            break;              // case TP_RIGHT_BORDER

        default:
            break;
        }
    }
    else                        // if ( b_isBorderBeingPressed )
    {
        cursorPositionType = TP::getCursorPositionType( this, eventPosition );
        switch ( cursorPositionType )
        {
        case TP::notAtBorder:
            if ( b_isCursorResize )
            {
                setCursor( QCursor ( Qt::ArrowCursor ) );
                b_isCursorResize = false;
            }
            break;

        case TP::leftBorder:
        case TP::rightBorder:
            if ( ! b_isCursorResize )
            {
                setCursor( QCursor( Qt::SizeHorCursor ) );
                b_isCursorResize = true;
            }
            break;

        default:
            break;
        }
    }

    QWidget::mouseMoveEvent( event );
}


void
TP_MainWindow::mouseReleaseEvent( QMouseEvent *event )
{
    if( b_isBorderBeingPressed )
    {
        b_isBorderBeingPressed = false;
        if( TP::getCursorPositionType( this, event->position().toPoint() ) == TP::notAtBorder
                && b_isCursorResize )
        {
            setCursor( QCursor( Qt::ArrowCursor ) );
            b_isCursorResize = false;
        }
        emit signal_windowChanged();
    }

    QWidget::mouseReleaseEvent( event );
}


// *****************************************************************
// private
// *****************************************************************


void
TP_MainWindow::initializeConnection()
{
    // System tray icon activation
    connect( trayIcon,  &QSystemTrayIcon::activated,
             this,      &TP_MainWindow::slot_trayIcon_activated );

    // Windows moving
    connect( ui->frame_Title,   &TP_TitleBar::signal_moveTitleBar,
             this,              &TP_MainWindow::slot_titleBarMoved );
    connect( ui->frame_Title,   &TP_TitleBar::signal_leftButtonReleased,
             this,              &TP_MainWindow::slot_leftButtonReleased );

    // Playback position jumping
    connect( ui->slider_Time,   &TP_TimeSlider::signal_mouseReleased,
             this,              &TP_MainWindow::slot_timeSliderPressed );

    // Volume setting
    connect( ui->label_VolumeIcon,  &TP_VolumeIcon::signal_setVolume,
             ui->slider_Volume,     &QSlider::setValue );
}


void
TP_MainWindow::initializeMenu()
{
    // ============================== System tray icon ==============================
    menu_trayIcon = new QMenu { this };

    menu_trayIcon->addAction( ui->action_trayIcon_Restore );
    menu_trayIcon->addAction( ui->action_trayIcon_Exit );

    trayIcon->setContextMenu( menu_trayIcon );

    // ============================== Playback mode button ==============================
    ui->action_setMode_SingleTime   ->setIcon( QIcon{ ":/image/icon_SingleTime.svg" } );
    ui->action_setMode_Repeat       ->setIcon( QIcon{ ":/image/icon_Repeat.svg" } );
    ui->action_setMode_Sequential   ->setIcon( QIcon{ ":/image/icon_Sequential.svg" } );
    ui->action_setMode_Shuffle      ->setIcon( QIcon{ ":/image/icon_Shuffle.svg" } );

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

    trayIcon->setIcon( QIcon{ ":/image/MusicalNote.svg" } );

    ui->label_AudioInfo->initialize();
    ui->label_AudioInfo->setFont( TP::config().getAudioInfoLabelFont() );
    setAudioInfoLabel();

    setAudioPropertyLabels();

    setWindowIcon( QIcon{ ":/image/MusicalNote.svg" } );
    ui->label_Title->setText( QString{ "ThousandPlayer v" } + TP_PROJECT_VERSION );

    ui->pushButton_Minimize ->setIcon( QIcon{ ":/image/icon_Minimize.svg" } );
    ui->pushButton_Expand   ->setIcon( QIcon{ ":/image/icon_Expand.svg" } );
    ui->pushButton_Exit     ->setIcon( QIcon{ ":/image/icon_Exit.svg" } );

    ui->label_VolumeIcon->initialize();

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

    setCursor( QCursor ( Qt::ArrowCursor ) );
}


void
TP_MainWindow::setIcon_Play()
{
    ui->pushButton_Play->setIcon( QIcon{ ":/image/icon_Play.svg" } );
    ui->pushButton_Play->setIconSize( QSize( TP::iconSize_Play, TP::iconSize_Play ) );
    ui->pushButton_Play->setToolTip( tr( "Play" ) );
}


void
TP_MainWindow::setIcon_Pause()
{
    ui->pushButton_Play->setIcon( QIcon{ ":/image/icon_Pause.svg" } );
    ui->pushButton_Play->setIconSize( QSize( TP::iconSize_Pause, TP::iconSize_Pause ) );
    ui->pushButton_Play->setToolTip( tr( "Pause" ) );
}


void
TP_MainWindow::setAudioInfoLabel( const QListWidgetItem *I_item )
{
    if( ! I_item )
        ui->label_AudioInfo->setStrings( { tr( "Idle" ) } );
    else
    {
        std::vector < QString > vec_Qstr { I_item->data( TP::role_Description ).toString() };

        if( ! I_item->data( TP::role_Title ).toString().isEmpty() )
            vec_Qstr.push_back( tr( "Title" ) + QString( ": " )
                                + I_item->data( TP::role_Title ).toString() );
        if( ! I_item->data( TP::role_Artist ).toString().isEmpty() )
            vec_Qstr.push_back( tr( "Artist" ) + QString( ": " )
                                + I_item->data( TP::role_Artist ).toString() );
        if( ! I_item->data( TP::role_Album ).toString().isEmpty() )
            vec_Qstr.push_back( tr( "Album" ) + QString( ": " )
                                + I_item->data( TP::role_Album ).toString() );

        ui->label_AudioInfo->setStrings( std::move( vec_Qstr ) );
    }
}


void
TP_MainWindow::setAudioPropertyLabels(
        const QString & I_qstr_Format,
        int             I_bitDepth,
        int             I_sampleRate,
        int             I_bitRate,
        int             I_duration,
        float           I_replayGain )
{
    ui->label_Format->setText(
                QString{ " " }
                + I_qstr_Format
                + QString{ " " } );

    ui->label_BitDepth->setText(
                QString{ " " }
                + ( I_bitDepth == -1 ? QString{ "-" } : QString::number( I_bitDepth ) )
                + QString{ " bits " }
                );

    ui->label_SampleRate->setText(
                QString{ " " } +
                ( I_sampleRate == -1 ? QString{ "-" } : QString::number( I_sampleRate ) ) +
                QString{ " KHz " }
                );

    ui->label_Bitrate->setText(
                QString{ " " } +
                ( I_bitRate == -1 ? QString( "-" ) : QString::number( I_bitRate ) ) +
                QString{ " kbps " }
                );

    ui->label_DurationTime->setText( convertTime( I_duration ) );
    ui->slider_Time->setRange( 0, I_duration );

    if( I_replayGain )
        ui->label_ReplayGain->setText(
                    QString{ " " } +
                    ( I_replayGain > 0 ? QString{ "+" } : QString{} ) +
                    QString::number( I_replayGain, 'f', 2 ) +
                    QString{ " dB " }
                    );
    else
        ui->label_ReplayGain->setText( QString {" - dB "} );
}


QString
TP_MainWindow::convertTime( qint64 I_second ) const
{
    return QString( "%1:%2" )
            .arg( I_second / 60, 2, 10, QLatin1Char( '0' ) )
            .arg( I_second % 60, 2, 10, QLatin1Char( '0' ) );
}
