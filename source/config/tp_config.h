#ifndef TP_CONFIG_H
#define TP_CONFIG_H

#include "tp_globalenum.h"

#include <QObject>
#include <QPoint>
#include <QSettings>

class TP_Config : public QObject
{
    Q_OBJECT

public:
    explicit TP_Config( QObject *parent = nullptr );
    ~TP_Config();

    // ==================== BOOT group ====================
    void    setMainWindowPosition( const QPoint &input );
    QPoint  getMainWindowPosition() const;
    void    setPlaylistWindowPosition( const QPoint &input );
    QPoint  getPlaylistWindowPosition() const;
    void    setPlaylistWindowShown( bool b );
    bool    isPlaylistWindowShown() const;

    // ==================== PLAYBACK group ====================
    void            setPlayMode( TP::PlayMode input );
    TP::PlayMode    getPlayMode() const;
    void            setVolume( int I_volume );
    int             getVolume() const;


private:
    const   QString     group_UI                        { "UI" };
    const   QString     key_UI_mainWindowPosition       { "mainWindowPosition" };
            QPoint      mainWindowPosition;
    const   QString     key_UI_playlistWindowPosition   { "playlistWindowPosition" };
            QPoint      playlistWindowPosition;
    const   QString     key_UI_isPlaylistWindowShown    { "isPlaylistWindowShown" };
            bool        b_isPlaylistWindowShown;

    const   QString         group_PLAYBACK                  { "PLAYBACK" };
    const   QString         key_PLAYBACK_volume             { "volume" };
            int             volume;
    const   QString         key_PLAYBACK_playMode           { "playMode" };
            TP::PlayMode    playMode;


    QSettings config;
};

#endif // TP_CONFIG_H
