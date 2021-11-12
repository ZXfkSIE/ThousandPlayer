#include "tp_config.h"

#include "tp_globalconst.h"

#include <QDir>
#include <QSettings>
#include <QWidget>

TP_Config::TP_Config( QObject *parent ) :
    QObject { parent }
  , config  { QSettings::IniFormat,
              QSettings::UserScope,
              QString { "ThousandPlayer" },
              QString { "ThousandPlayer" },
              this }
{
    qDebug() << "[Config] The path of config file is" << config.fileName();

    QFont defaultFont = QWidget().font();
    defaultFont.setPointSize( 10 );

    config.beginGroup( group_MAINWINDOW );

    mainWindowPosition = config
            .value( key_MAINWINDOW_mainWindowPosition, QPoint { 100, 100 } )
            .toPoint();

    b_isTrayIconEnabled = config
            .value( key_MAINWINDOW_isTrayIconEnabled, false )
            .toBool();

    audioInfoLabelFont = config
            .value( key_MAINWINDOW_audioInfoLabelFont, defaultFont )
            .value< QFont >();

    audioInfoScrollingInterval_sec = config
            .value( key_MAINWINDOW_audioInfoScrollingInterval, 5 )
            .toInt();

    config.endGroup();


    config.beginGroup( group_PLAYLISTWINDOW );

    playlistWindowPosition = config
            .value( key_PLAYLISTWINDOW_playlistWindowPosition, QPoint { 100, 340 } )
            .toPoint();

    b_isPlaylistWindowShown = config
            .value( key_PLAYLISTWINDOW_isPlaylistWindowShown, true )
            .toBool();


    playlistFont = config
            .value( key_PLAYLISTWINDOW_playlistFont, defaultFont )
            .value< QFont >();

    lastOpenedDirectory = config
            .value( key_PLAYLISTWINDOW_lastOpenedDirectory, QUrl::fromLocalFile( QDir::homePath() ) )
            .toUrl();

    config.endGroup();


    config.beginGroup( group_PLAYBACK );

    volume = config
            .value( key_PLAYBACK_volume, 50 )
            .toInt();

    preAmp_dB = config
            .value( key_PLAYBACK_preAmp_dB, 0.0 )
            .toFloat();

    replayGainMode = static_cast< TP::ReplayGainMode >(
                config
                .value( key_PLAYBACK_replayGainMode, TP::RG_disabled )
                .toInt()
                );

    defaultGain_dB = config
            .value( key_PLAYBACK_defaultGain_dB, 0.0 )
            .toFloat();

    playMode = static_cast<TP::PlayMode>(
                config
                .value( key_PLAYBACK_playMode, TP::singleTime )
                .toInt()
                );

    config.endGroup();
}

TP_Config::~TP_Config()
{
    config.beginGroup( group_MAINWINDOW );
    config.setValue( key_MAINWINDOW_mainWindowPosition, mainWindowPosition );
    config.setValue( key_MAINWINDOW_isTrayIconEnabled, b_isTrayIconEnabled );
    config.setValue( key_MAINWINDOW_audioInfoLabelFont, audioInfoLabelFont );
    config.setValue( key_MAINWINDOW_audioInfoScrollingInterval, audioInfoScrollingInterval_sec );
    config.endGroup();

    config.beginGroup( group_PLAYLISTWINDOW );
    config.setValue( key_PLAYLISTWINDOW_playlistWindowPosition, playlistWindowPosition );
    config.setValue( key_PLAYLISTWINDOW_isPlaylistWindowShown, b_isPlaylistWindowShown );
    config.setValue( key_PLAYLISTWINDOW_playlistFont, playlistFont );
    config.setValue( key_PLAYLISTWINDOW_lastOpenedDirectory, lastOpenedDirectory );
    config.endGroup();

    config.beginGroup( group_PLAYBACK );
    config.setValue( key_PLAYBACK_volume, volume );
    config.setValue( key_PLAYBACK_preAmp_dB, preAmp_dB );
    config.setValue( key_PLAYBACK_replayGainMode, replayGainMode );
    config.setValue( key_PLAYBACK_defaultGain_dB, defaultGain_dB );
    config.setValue( key_PLAYBACK_playMode, playMode );
    config.endGroup();

    config.sync();
}

// ==================== UI group ====================
void
TP_Config::setMainWindowPosition( const QPoint &input )
{
    mainWindowPosition = input;
}

QPoint
TP_Config::getMainWindowPosition() const
{
    return mainWindowPosition;
}

void
TP_Config::setTrayIconEnabled ( const bool b )
{
    b_isTrayIconEnabled = b;
}

bool
TP_Config::isTrayIconEnabled ()
{
    return b_isTrayIconEnabled;
}

void
TP_Config::setAudioInfoLabelFont( const QFont &input )
{
    audioInfoLabelFont = input;
}

QFont
TP_Config::getAudioInfoLabelFont()
{
    return audioInfoLabelFont;
}

void
TP_Config::setAudioInfoScrollingInterval ( const int I_sec )
{
    audioInfoScrollingInterval_sec = I_sec;
}

int
TP_Config::getAudioInfoScrollingInterval () const
{
    return audioInfoScrollingInterval_sec;
}

void
TP_Config::setPlaylistWindowPosition( const QPoint &input )
{
    playlistWindowPosition = input;
}

QPoint
TP_Config::getPlaylistWindowPosition() const
{
    return playlistWindowPosition;
}

void
TP_Config::setPlaylistWindowShown( bool b )
{
    b_isPlaylistWindowShown = b;
}

bool
TP_Config::isPlaylistWindowShown() const
{
    return b_isPlaylistWindowShown;
}

void
TP_Config::setPlaylistFont( const QFont &input )
{
    playlistFont = input;
}

QFont
TP_Config::getPlaylistFont() const
{
    return playlistFont;
}

void
TP_Config::setLastOpenedDirectory ( const QUrl &input )
{
    lastOpenedDirectory = input;
}

QUrl
TP_Config::getLastOpenedDirectory () const
{
    return lastOpenedDirectory;
}

// ==================== PLAYBACK group ====================
void
TP_Config::setVolume( const int I_volume )
{
    volume = I_volume;
}

int
TP_Config::getVolume() const
{
    return volume;
}

void
TP_Config::setPreAmp_dB( const float I_dB )
{
    preAmp_dB = I_dB;
}

float
TP_Config::getPreAmp_dB() const
{
    return preAmp_dB;
}

void
TP_Config::setReplayGainMode( const TP::ReplayGainMode input )
{
    replayGainMode = input;
}

TP::ReplayGainMode
TP_Config::getReplayGainMode() const
{
    return replayGainMode;
}

void
TP_Config::setDefaultGain_dB( const float I_dB )
{
    defaultGain_dB = I_dB;
}

float
TP_Config::getDefaultGain_dB() const
{
    return defaultGain_dB;
}

void
TP_Config::setPlayMode( const TP::PlayMode input )
{
    playMode = input;
}

TP::PlayMode
TP_Config::getPlayMode() const
{
    return playMode;
}
