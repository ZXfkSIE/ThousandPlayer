#include "tp_config.h"

#include "tp_globalconst.h"

#include <QSettings>

TP_Config::TP_Config( QObject *parent ) :
    QObject { parent }
  , config  { TP::configFilePath, QSettings::IniFormat, this }
{
    config.beginGroup( group_UI );
    mainWindowPosition = config
            .value( key_UI_mainWindowPosition, QPoint { 100, 100 } )
            .toPoint();
    playlistWindowPosition = config
            .value( key_UI_playlistWindowPosition, QPoint { 100, 340 } )
            .toPoint();
    b_isPlaylistWindowShown = config
            .value( key_UI_isPlaylistWindowShown, true )
            .toBool();
    config.endGroup();

    config.beginGroup( group_PLAYBACK );


    volume = config
            .value( key_PLAYBACK_volume, 50 )
            .toInt();
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
    config.setValue( key_UI_playlistWindowPosition, playlistWindowPosition );
    config.setValue( key_UI_isPlaylistWindowShown, b_isPlaylistWindowShown );
    config.endGroup();

    config.beginGroup( group_PLAYBACK );
    config.setValue( key_PLAYBACK_volume, volume );
    config.setValue( key_PLAYBACK_playMode, playMode );
    config.endGroup();

    config.sync();
}

// UI group
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


// PLAYBACK group
void
TP_Config::setPlayMode( TP::PlayMode input )
{
    playMode = input;
}

TP::PlayMode
TP_Config::getPlayMode() const
{
    return playMode;
}

void
TP_Config::setVolume( int I_volume )
{
    volume = I_volume;
}

int
TP_Config::getVolume() const
{
    return volume;
}
