#include "tp_configwindow.h"
#include "ui_tp_configwindow.h"

#include "tp_cmakeconfig.h"
#include "tp_globalvariable.h"

#include <QAudioDevice>
#include <QFontDialog>
#include <QMediaDevices>

TP_ConfigWindow::TP_ConfigWindow( QWidget *parent ) :
    QDialog { parent }
  , ui      { new Ui::TP_ConfigWindow }
{
    ui->setupUi( this );

    initializeConnection();
    initializeUI();
}


TP_ConfigWindow::~TP_ConfigWindow()
{
    delete ui;
}

// *****************************************************************
// private slots:
// *****************************************************************

void
TP_ConfigWindow::slot_switchPage( const int currentRow )
{
    ui->stackedWidget->setCurrentIndex( currentRow );
}


void
TP_ConfigWindow::slot_audioDeviceChanged( const int index )
{
    qDebug() << "[Config Window] signal_audioDeviceChanged is emitted. Audio device shoule be changed to"
             << ui->comboBox_AudioDevice->itemData( index ).value< QAudioDevice >().description();
    emit signal_audioDeviceChanged(
                ui->comboBox_AudioDevice->itemData( index ).value< QAudioDevice >()
                );
}


void
TP_ConfigWindow::slot_ReplayGainModeChanged( const int index )
{
    qDebug() << "[Config Window] ReplayGain mode is changed to" << index;
    switch( index )
    {
    case 0:
        ui->slider_PreAmp           ->setEnabled( false );
        ui->slider_DefaultReplayGain->setEnabled( false );
        TP::config().setReplayGainMode( TP::RG_disabled );
        break;

    case 1:
        ui->slider_PreAmp           ->setEnabled( true );
        ui->slider_DefaultReplayGain->setEnabled( true );
        TP::config().setReplayGainMode( TP::RG_track );
        break;

    case 2:
        ui->slider_PreAmp           ->setEnabled( true );
        ui->slider_DefaultReplayGain->setEnabled( true );
        TP::config().setReplayGainMode( TP::RG_album );
        break;
    }
}


void
TP_ConfigWindow::slot_setPreAmp( const int value )
{
    TP::config().setPreAmp_dB( value / 10.0 );
}


void
TP_ConfigWindow::slot_setDefaultReplayGain( const int value )
{
    TP::config().setDefaultGain_dB( value / 10.0 );
}


void
TP_ConfigWindow::on_pushButton_ChangePlaylistFont_clicked()
{
    bool ok {};
    QFont font {
        QFontDialog::getFont(
                    &ok,
                    TP::config().getPlaylistFont(),
                    this )
    };

    if( ok )
    {
        TP::config().setPlaylistFont( font );
        ui->label_CurrentPlaylistFontExample->setText(
                    QString( "%1, %2 pt" ).arg( font.family() ).arg( font.pointSize() ) );
        ui->label_CurrentPlaylistFontExample->setFont( font );
        emit signal_fontChanged();
    }
}

// *****************************************************************
// private
// *****************************************************************

void
TP_ConfigWindow::initializeConnection()
{
    connect( ui->listWidget_Tab,    &QListWidget::currentRowChanged,
             this,                  &TP_ConfigWindow::slot_switchPage );

    connect( ui->comboBox_AudioDevice,      &QComboBox::currentIndexChanged,
             this,                          &TP_ConfigWindow::slot_audioDeviceChanged );
    connect( ui->comboBox_ReplayGainMode,   &QComboBox::currentIndexChanged,
             this,                          &TP_ConfigWindow::slot_ReplayGainModeChanged );
    connect( ui->slider_PreAmp, &QSlider::valueChanged,
             this,              &TP_ConfigWindow::slot_setPreAmp );
}

void
TP_ConfigWindow::initializeUI()
{
    // ============================== Tab list ==============================

    ui->listWidget_Tab->setCurrentRow( 0 );
    ui->stackedWidget->setCurrentIndex( 0 );

    for( int i {}; i < ui->listWidget_Tab->count(); i++ )
        ui->listWidget_Tab->item( i )->setSizeHint( QSize{ 0, 40 } );

    // ============================== Playback page ==============================

    // Initialize list of audio output device
    ui->comboBox_AudioDevice->addItem( tr( "Default" ), QVariant::fromValue( QAudioDevice() ) );
    for ( const auto &deviceInfo: QMediaDevices::audioOutputs() )
        ui->comboBox_AudioDevice->addItem( deviceInfo.description(), QVariant::fromValue( deviceInfo ) );
    ui->comboBox_AudioDevice->setCurrentIndex( 0 );

    // Initialize ReplayGain mode ComboBox
    switch( TP::config().getReplayGainMode() )
    {
    case TP::RG_disabled :
        ui->slider_PreAmp           ->setEnabled( false );
        ui->slider_DefaultReplayGain->setEnabled( false );
        break;

    case TP::RG_track :
        ui->comboBox_ReplayGainMode->setCurrentIndex( 1 );
        break;

    case TP::RG_album :
        ui->comboBox_ReplayGainMode->setCurrentIndex( 2 );
        break;
    }

    // Initialize sliders
    ui->slider_PreAmp           ->setValue( TP::config().getPreAmp_dB()     * 10 );
    ui->slider_DefaultReplayGain->setValue( TP::config().getDefaultGain_dB()* 10 );

    // ============================== Playlist page ==============================

    QFont playlistFont { TP::config().getPlaylistFont() };
    ui->label_CurrentPlaylistFontExample->setText(
                QString( "%1, %2 pt" ).arg( playlistFont.family() ).arg( playlistFont.pointSize() ) );
    ui->label_CurrentPlaylistFontExample->setFont( playlistFont );

    // ============================== About page ==============================
    ui->label_Icon->setPixmap( QIcon{ ":/image/MusicalNote.svg" }
                               .pixmap( ui->label_Icon->size() )
                               );
    ui->label_SoftName->setText( QString("ThousandPlayer v") + TP_PROJECT_VERSION );
}
