#ifndef TP_CONFIG_H
#define TP_CONFIG_H

#include "tp_globalenum.h"

#include <QFont>
#include <QObject>
#include <QPoint>
#include <QSettings>
#include <QUrl>

class TP_Config : public QObject
{
    Q_OBJECT

public:
    explicit TP_Config( QObject *parent = nullptr );
    ~TP_Config();

    // ====================== MAINWINDOW group ======================
    void            setMainWindowPosition ( const QPoint &input = defaultMainWindowPosition );
    const QPoint &  getMainWindowPosition () const;
    void            setTrayIconEnabled  ( const bool input );
    bool            isTrayIconEnabled   () const;
    void            setAudioInfoLabelFont( const QFont &input );
    const QFont &   getAudioInfoLabelFont() const;
    void            setAudioInfoScrollingInterval ( const int I_sec );
    int             getAudioInfoScrollingInterval () const;

    // ==================== PLAYLISTWINDOW group ====================
    void            setPlaylistWindowPosition ( const QPoint &input = defaultPlaylistWindowPosition );
    const QPoint &  getPlaylistWindowPosition () const;
    void            setPlaylistWindowShown  ( const bool input );
    bool            isPlaylistWindowShown   () const;
    void            setPlaylistFont ( const QFont &input );
    const QFont &   getPlaylistFont () const;
    void            setLastOpenedDirectory ( const QUrl &input );
    const QUrl &    getLastOpenedDirectory () const;

    // ===================== LYRICSWINDOW group =====================
    void            setLyricsWindowPosition ( const QPoint &input = defaultLyricsWindowPosition );
    const QPoint &  getLyricsWindowPosition () const;
    void            setLyricsWindowShown  ( const bool input );
    bool            isLyricsWindowShown   () const;
    void            setLyricsFont ( const QFont &input );
    const QFont &   getLyricsFont () const;

    // ======================= PLAYBACK group =======================
    void                setVolume ( const int input );
    int                 getVolume () const;
    void                setPreAmp_dB ( const float input );
    float               getPreAmp_dB () const;
    void                setReplayGainMode ( const TP::ReplayGainMode input );
    TP::ReplayGainMode  getReplayGainMode () const;
    void                setDefaultGain_dB ( const float input );
    float               getDefaultGain_dB () const;
    void                setPlayMode ( const TP::PlayMode input );
    TP::PlayMode        getPlayMode () const;

private:
    QSettings config;


    // ************************************************************************************
    const   QString     group_MAINWINDOW                            { "MAINWINDOW" };
    // ************************************************************************************
    const   QString     key_MAINWINDOW_mainWindowPosition           { "mainWindowPosition" };
            QPoint      mainWindowPosition;
    const   QString     key_MAINWINDOW_isTrayIconEnabled            { "isTrayIconEnabled" };
            bool        b_isTrayIconEnabled;
    const   QString     key_MAINWINDOW_audioInfoLabelFont           { "audioInfoLabelFont " };
            QFont       audioInfoLabelFont;
    const   QString     key_MAINWINDOW_audioInfoScrollingInterval   { "audioInfoScrollingInterval" };
            int         audioInfoScrollingInterval_sec;

    // ************************************************************************************
    const   QString     group_PLAYLISTWINDOW                        { "PLAYLISTWINDOW" };
    // ************************************************************************************
    const   QString     key_PLAYLISTWINDOW_playlistWindowPosition   { "playlistWindowPosition" };
            QPoint      playlistWindowPosition;
    const   QString     key_PLAYLISTWINDOW_isPlaylistWindowShown    { "isPlaylistWindowShown" };
            bool        b_isPlaylistWindowShown;
    const   QString     key_PLAYLISTWINDOW_playlistFont             { "playlistFont" };
            QFont       playlistFont;
    const   QString     key_PLAYLISTWINDOW_lastOpenedDirectory      { "lastOpenedDirectory" };
            QUrl        lastOpenedDirectory;

    // ************************************************************************************
    const   QString             group_LYRICSWINDOW                  { "LYRICSWINDOW" };
    // ************************************************************************************
    const   QString     key_LYRICSWINDOW_lyricsWindowPosition       { "lyricsWindowPosition" };
            QPoint      lyricsWindowPosition;
    const   QString     key_LYRICSWINDOW_isLyricsWindowShown        { "isLyricsWindowShown" };
            bool        b_isLyricsWindowShown;
    const   QString     key_LYRICSWINDOW_lyricsFont                 { "lyricsFont" };
            QFont       lyricsFont;

    // ************************************************************************************
    const   QString             group_PLAYBACK                      { "PLAYBACK" };
    // ************************************************************************************
    const   QString             key_PLAYBACK_volume                 { "volume" };
            int                 volume;
    const   QString             key_PLAYBACK_preAmp_dB              { "preAmp_dB" };
            float               preAmp_dB;
    const   QString             key_PLAYBACK_replayGainMode         { "replayGainMode" };
            TP::ReplayGainMode  replayGainMode;
    const   QString             key_PLAYBACK_defaultGain_dB         { "defaultGain_dB" };
            float               defaultGain_dB;
    const   QString             key_PLAYBACK_playMode               { "playMode" };
            TP::PlayMode        playMode;

    static constexpr QPoint defaultMainWindowPosition       { 100, 100 };
    static constexpr QPoint defaultPlaylistWindowPosition   { 100, 340 };
    static constexpr QPoint defaultLyricsWindowPosition     { 580, 100 };
};

#endif // TP_CONFIG_H
