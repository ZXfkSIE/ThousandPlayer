#include "tp_configwindow.h"
#include "ui_tp_configwindow.h"

#include "tp_cmakeconfig.h"
#include "tp_globalvariable.h"

#include <QAudioDevice>
#include <QFileDialog>
#include <QFontDialog>
#include <QMediaDevices>

TP_ConfigWindow::TP_ConfigWindow( QWidget *parent ) :
    QDialog { parent }
  , ui      { new Ui::TP_ConfigWindow }
{
    ui->setupUi( this );

    setWindowFlag( Qt::WindowStaysOnTopHint );

    initializeStrings();
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
TP_ConfigWindow::on_listWidget_Tab_currentRowChanged( int I_row )
{
    ui->stackedWidget->setCurrentIndex( I_row );
}


void
TP_ConfigWindow::on_radioButton_ClickX_MinimizeToTray_toggled( bool I_checked )
{
    TP::config().setTrayIconEnabled( I_checked );
}


void TP_ConfigWindow::on_pushButton_ChangeAudioInfoLabelFont_clicked()
{
    bool ok {};
    const auto &font {
        QFontDialog::getFont(
                    &ok,
                    TP::config().getAudioInfoLabelFont(),
                    this )
    };

    if( ok )
    {
        TP::config().setAudioInfoLabelFont( font );
        ui->label_CurrentAudioInfoLabelFontExample->setText(
                    QString( "%1, %2 pt" ).arg( font.family() ).arg( font.pointSize() ) );
        ui->label_CurrentAudioInfoLabelFontExample->setFont( font );
        emit signal_audioInfoLabelFontChanged();
    }
}


void
TP_ConfigWindow::on_spinBox_AudioInfoLabelScrollingInterval_valueChanged( int I_sec )
{
    TP::config().setAudioInfoScrollingInterval( I_sec );
}


void
TP_ConfigWindow::on_comboBox_AudioDevice_currentIndexChanged( int I_index )
{
    qDebug() << "[Config Window] signal_audioDeviceChanged is emitted to"
             << ui->comboBox_AudioDevice->itemData( I_index ).value< QAudioDevice >().description();
    emit signal_audioDeviceChanged(
                ui->comboBox_AudioDevice->itemData( I_index ).value< QAudioDevice >()
                );
}


void
TP_ConfigWindow::on_comboBox_ReplayGainMode_currentIndexChanged( int I_index )
{
    qDebug() << "[Config Window] ReplayGain mode is changed to" << I_index;
    switch( I_index )
    {
    case 0:
        ui->slider_PreAmp           ->setEnabled( false );
        ui->slider_DefaultReplayGain->setEnabled( false );
        TP::config().setReplayGainMode( TP::ReplayGainMode::Disabled );
        break;

    case 1:
        ui->slider_PreAmp           ->setEnabled( true );
        ui->slider_DefaultReplayGain->setEnabled( true );
        TP::config().setReplayGainMode( TP::ReplayGainMode::Track );
        break;

    case 2:
        ui->slider_PreAmp           ->setEnabled( true );
        ui->slider_DefaultReplayGain->setEnabled( true );
        TP::config().setReplayGainMode( TP::ReplayGainMode::Album );
        break;
    }
}


void
TP_ConfigWindow::on_slider_PreAmp_valueChanged( int I_value )
{
    float realValue = I_value / 10.0;
    TP::config().setPreAmp_dB( realValue );
    ui->label_PreAmpValue->setText(
                ( realValue > 0 ? QString( "+" ) : QString() ) +
                QString( "%1 dB" ).arg( realValue ) );
}


void
TP_ConfigWindow::on_slider_DefaultReplayGain_valueChanged( int I_value )
{
    float realValue = I_value / 10.0;
    TP::config().setDefaultGain_dB( realValue );
    ui->label_DefaultReplayGainValue->setText(
                ( realValue > 0 ? QString( "+" ) : QString() ) +
                QString( "%1 dB" ).arg( realValue ) );
}


void
TP_ConfigWindow::on_lineEdit_RsgainPath_textChanged( const QString & I_path )
{
    TP::config().setRsgainPath( I_path );
}


void
TP_ConfigWindow::on_pushButton_RsgainPath_clicked()
{
    const auto &path {
        QFileDialog::getOpenFileName(
                    this,                                                   // QWidget *parent = nullptr
                    {},                                                     // const QString &caption = QString()
                    {},                                                     // const QString &dir = QString()
                    tr( "Executable files" ) + QString( " (*.exe);;" )      // const QString &filter = QString()
                    )
    };

    if( ! path.isEmpty() )
        ui->lineEdit_RsgainPath->setText( path );
}


void
TP_ConfigWindow::on_pushButton_ChangePlaylistFont_clicked()
{
    bool ok {};
    const auto &font {
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
        emit signal_playlistFontChanged();
    }
}


void
TP_ConfigWindow::on_pushButton_ChangeLyricsFont_clicked()
{
    bool ok {};
    const auto &font {
        QFontDialog::getFont(
                    &ok,
                    TP::config().getLyricsFont(),
                    this )
    };

    if( ok )
    {
        TP::config().setLyricsFont( font );
        ui->label_CurrentLyricsFontExample->setText(
                    QString( "%1, %2 pt" ).arg( font.family() ).arg( font.pointSize() ) );
        ui->label_CurrentLyricsFontExample->setFont( font );
        emit signal_lyricsFontChanged();
    }
}


void
TP_ConfigWindow::on_spinBox_JumpingTimeOffset_valueChanged( int I_ms )
{
    TP::config().setJumpingTimeOffset_ms( I_ms );
}

// *****************************************************************
// private
// *****************************************************************

void
TP_ConfigWindow::initializeStrings()
{
    QFile qFile_Credits { ":/markdown/credits.md" };
    if( qFile_Credits.open( QIODeviceBase::ReadOnly | QIODeviceBase::Text ) )
        qstr_Credits = QTextStream { &qFile_Credits }.readAll();

    QFile qFile_Licenses { ":/markdown/gpl-3.0.md" };
    if( qFile_Licenses.open( QIODeviceBase::ReadOnly | QIODeviceBase::Text ) )
        qstr_Licenses = QTextStream { &qFile_Licenses }.readAll();
}


void
TP_ConfigWindow::initializeUI()
{
    setWindowIcon( QIcon{ ":/image/icon_Config.svg" } );

    // ============================== Tab list ==============================

    ui->listWidget_Tab->setCurrentRow( 0 );
    ui->stackedWidget->setCurrentIndex( 0 );

    // Expand height of list items
    for( int i {}; i < ui->listWidget_Tab->count(); i++ )
        ui->listWidget_Tab->item( i )->setSizeHint( { 0, 40 } );

    // ============================== Main Page ==============================

    // When click the X button
    if( TP::config().isTrayIconEnabled() )
        ui->radioButton_ClickX_MinimizeToTray->setChecked( true );
    else
        ui->radioButton_ClickX_Quit->setChecked( true );

    // Audio information label font
    const auto &audioInfoLabelFont { TP::config().getAudioInfoLabelFont() };
    ui->label_CurrentAudioInfoLabelFontExample->setText(
                QString { "%1, %2 pt" }.arg( audioInfoLabelFont.family() ).arg( audioInfoLabelFont.pointSize() ) );
    ui->label_CurrentAudioInfoLabelFontExample->setFont( audioInfoLabelFont );

    // Audio information label font scrolling interval
    on_spinBox_AudioInfoLabelScrollingInterval_valueChanged( TP::config().getAudioInfoScrollingInterval() );

    // ============================== Playback page ==============================

    // List of audio output device
    ui->comboBox_AudioDevice->addItem( tr( "Default device" ), QVariant::fromValue( QAudioDevice {} ) );
    for ( const auto &deviceInfo: QMediaDevices::audioOutputs() )
        ui->comboBox_AudioDevice->addItem( deviceInfo.description(), QVariant::fromValue( deviceInfo ) );
    on_comboBox_AudioDevice_currentIndexChanged( 0 );

    // ReplayGain mode ComboBox
    switch( TP::config().getReplayGainMode() )
    {
    case TP::ReplayGainMode::Disabled :
        on_comboBox_ReplayGainMode_currentIndexChanged( 0 );
        break;

    case TP::ReplayGainMode::Track :
        on_comboBox_ReplayGainMode_currentIndexChanged( 1 );
        break;

    case TP::ReplayGainMode::Album :
        on_comboBox_ReplayGainMode_currentIndexChanged( 2 );
        break;
    }

    // Sliders
    on_slider_PreAmp_valueChanged           ( TP::config().getPreAmp_dB() );
    on_slider_DefaultReplayGain_valueChanged( TP::config().getDefaultGain_dB() );

    // Path of rsgain (https://github.com/complexlogic/rsgain)
#ifdef Q_OS_WIN
    ui->lineEdit_RsgainPath->setText( TP::config().getRsgainPath() );
    ui->pushButton_RsgainPath->setIcon( QIcon{ ":/image/icon_OpenFile.svg" } );
#else
    ui->container_RsgainPath->setVisible( false );
#endif

    // ============================== Playlist page ==============================

    const auto &playlistFont { TP::config().getPlaylistFont() };
    ui->label_CurrentPlaylistFontExample->setText(
                QString( "%1, %2 pt" ).arg( playlistFont.family() ).arg( playlistFont.pointSize() ) );
    ui->label_CurrentPlaylistFontExample->setFont( playlistFont );

    // ============================== Lyrics page ==============================

    const auto &lyricsFont { TP::config().getLyricsFont() };
    ui->label_CurrentLyricsFontExample->setText(
                QString( "%1, %2 pt" ).arg( lyricsFont.family() ).arg( lyricsFont.pointSize() ) );
    ui->label_CurrentLyricsFontExample->setFont( lyricsFont );

    // ============================== About page ==============================

    ui->label_Icon->setPixmap( QIcon{ ":/image/MusicalNote.svg" }
                               .pixmap( ui->label_Icon->size() )
                               );
    ui->label_SoftName->setText( QString( "ThousandPlayer v" ) + TP::projectVersion );
}


void
TP_ConfigWindow::on_pushButton_Credits_clicked()
{
    ui->textBrowser->document()->setMarkdown( qstr_Credits );
}


void
TP_ConfigWindow::on_pushButton_Licenses_clicked()
{
    ui->textBrowser->document()->setMarkdown( qstr_Licenses );
}


void
TP_ConfigWindow::on_buttonBox_OK_accepted()
{
    close();
}
