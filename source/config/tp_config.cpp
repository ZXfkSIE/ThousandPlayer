#include "tp_config.h"

#include "tp_globalconst.h"

#include <QSettings>

TP_Config::TP_Config( QObject *parent ) :
    QObject { parent }
  , config { TP::configFilePath, QSettings::IniFormat, this }
{
    config.beginGroup( group_UI );
    mainWindowPosition = config.value( key_UI_mainWindowPosition,
                QPoint { 100, 100 }
                ).value<QPoint>();
    playlistWindowPosition = config.value( key_UI_playlistWindowPosition,
                QPoint { 100, 340 }
                ).value<QPoint>();
    config.endGroup();

    config.beginGroup( group_PLAYBACK );

    playMode = static_cast<TP::PlayMode>(
                config.value( key_PLAYBACK_playMode, TP::singleTime ).toInt()
                );
    config.endGroup();
}

TP_Config::~TP_Config()
{
    config.beginGroup( group_UI );
    config.setValue( key_UI_mainWindowPosition, mainWindowPosition );
    config.setValue( key_UI_playlistWindowPosition, playlistWindowPosition );
    config.endGroup();

    config.beginGroup( group_PLAYBACK );
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
