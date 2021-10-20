#include "tp_configwindow.h"
#include "ui_tp_configwindow.h"

#include "tp_globalvariable.h"

#include <QAudioDevice>
#include <QMediaDevices>

TP_ConfigWindow::TP_ConfigWindow( QWidget *parent ) :
    QDialog { parent }
  , ui      { new Ui::TP_ConfigWindow }
{
    ui->setupUi( this );

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
TP_ConfigWindow::slot_audioDeviceChanged()
{

}

void
TP_ConfigWindow::slot_ReplayGainModeChanged()
{

}

// *****************************************************************
// private
// *****************************************************************

void
TP_ConfigWindow::initializeConnection()
{
    connect( ui->comboBox_AudioDevice,      &QComboBox::currentIndexChanged,
             this,                          &TP_ConfigWindow::slot_audioDeviceChanged );
    connect( ui->comboBox_ReplayGainMode,   &QComboBox::currentIndexChanged,
             this,                          &TP_ConfigWindow::slot_ReplayGainModeChanged );
}

void
TP_ConfigWindow::initializeUI()
{
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
        ui->comboBox_ReplayGainMode->setCurrentIndex( 0 );
        break;

    case TP::RG_track :
        ui->comboBox_ReplayGainMode->setCurrentIndex( 1 );
        break;

    case TP::RG_album :
        ui->comboBox_ReplayGainMode->setCurrentIndex( 2 );
        break;
    }
}
