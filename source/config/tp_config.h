#ifndef TP_CONFIG_H
#define TP_CONFIG_H

#include "tp_globalenum.h"

#include <QFont>
#include <QObject>
#include <QPoint>
#include <QSettings>

class TP_Config : public QObject
{
    Q_OBJECT

public:
    explicit TP_Config( QObject *parent = nullptr );
    ~TP_Config();

    // ==================== UI group ====================
    void    setMainWindowPosition( const QPoint &input );
    QPoint  getMainWindowPosition() const;
    void    setPlaylistWindowPosition( const QPoint &input );
    QPoint  getPlaylistWindowPosition() const;
    void    setPlaylistWindowShown( bool b );
    bool    isPlaylistWindowShown() const;
    void    setPlaylistFont( const QFont &input );
    QFont   getPlaylistFont();

    // ==================== PLAYBACK group ====================
    void                setVolume( int I_volume );
    int                 getVolume() const;
    void                setPreAmp_dB( float I_dB );
    float               getPreAmp_dB();
    void                setReplayGainMode( TP::ReplayGainMode input );
    TP::ReplayGainMode  getReplayGainMode();
    void                setDefaultGain_dB( float I_dB );
    float               getDefaultGain_dB();
    void                setPlayMode( TP::PlayMode input );
    TP::PlayMode        getPlayMode() const;


private:
    QSettings config;


    // ************************************************************************************
    const   QString     group_UI                        { "UI" };
    // ************************************************************************************
    const   QString     key_UI_mainWindowPosition       { "mainWindowPosition" };
            QPoint      mainWindowPosition;
    const   QString     key_UI_playlistWindowPosition   { "playlistWindowPosition" };
            QPoint      playlistWindowPosition;
    const   QString     key_UI_isPlaylistWindowShown    { "isPlaylistWindowShown" };
            bool        b_isPlaylistWindowShown;
    const   QString     key_UI_playlistFont             { "playlistFont" };
            QFont       playlistFont;

    // ************************************************************************************
    const   QString             group_PLAYBACK              { "PLAYBACK" };
    // ************************************************************************************

    const   QString             key_PLAYBACK_volume         { "volume" };
            int                 volume;
    const   QString             key_PLAYBACK_preAmp_dB      { "preAmp_dB" };
            float               preAmp_dB;
    const   QString             key_PLAYBACK_replayGainMode { "replayGainMode" };
            TP::ReplayGainMode  replayGainMode;
    const   QString             key_PLAYBACK_defaultGain_dB { "defaultGain_dB" };
            float               defaultGain_dB;
    const   QString             key_PLAYBACK_playMode       { "playMode" };
            TP::PlayMode        playMode;
};

#endif // TP_CONFIG_H
