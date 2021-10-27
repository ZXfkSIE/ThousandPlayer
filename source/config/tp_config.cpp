#include "tp_config.h"

#include "tp_globalconst.h"

#include <QSettings>
#include <QWidget>

TP_Config::TP_Config( QObject *parent ) :
    QObject { parent }
  , config  { TP::configFilePath, QSettings::IniFormat, this }
{
    config.beginGroup( group_UI );

    mainWindowPosition = config
            .value( key_UI_mainWindowPosition, QPoint { 100, 100 } )
            .toPoint();

    audioInfoScrollingInterval_sec = config
            .value( key_UI_audioInfoScrollingInterval, 5 )
            .toInt();

    playlistWindowPosition = config
            .value( key_UI_playlistWindowPosition, QPoint { 100, 340 } )
            .toPoint();

    b_isPlaylistWindowShown = config
            .value( key_UI_isPlaylistWindowShown, true )
            .toBool();

    QFont font = QWidget().font();
    font.setPointSize( 10 );
    playlistFont = config
            .value( key_UI_playlistFont, font  )
            .value< QFont >();

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
    config.beginGroup( group_UI );
    config.setValue( key_UI_mainWindowPosition, mainWindowPosition );
    config.setValue( key_UI_audioInfoScrollingInterval, audioInfoScrollingInterval_sec );
    config.setValue( key_UI_playlistWindowPosition, playlistWindowPosition );
    config.setValue( key_UI_isPlaylistWindowShown, b_isPlaylistWindowShown );
    config.setValue( key_UI_playlistFont, playlistFont );
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
