#ifndef TP_MAINCLASS_H
#define TP_MAINCLASS_H

#include "tp_globalconst.h"
#include "tp_globalenum.h"

#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QObject>

class TP_ConfigWindow;
class TP_FileListWidget;
class TP_LyricsWindow;
class TP_MainWindow;
class TP_PlaylistWindow;

class QAudioOutput;
class QListWidgetItem;

namespace TagLib
{
namespace APE   { class Tag; }
namespace FLAC  { class File; }
namespace ID3v2 { class Tag; }
namespace MP4   { class Tag; }
namespace Ogg   { class XiphComment; }
}

class TP_MainClass : public QObject
{
    Q_OBJECT

public:
    explicit TP_MainClass();
    ~TP_MainClass();

public slots:
    void slot_checkIfServiceAvailable();
    void slot_initializePosition();

private slots:
    void slot_exitProgram();
    void slot_minimizeWindow();
    void slot_restoreWindow();

    void slot_moveWindow ( QWidget *I_window, QRect I_geometry );
    void slot_resizeWindow ( QWidget *I_window, QRect I_geometry, TP::CursorPositionType I_resizeType );
    void slot_refreshSnapStatus();

    void slot_itemDoubleClicked ( QListWidgetItem *I_item );

    void slot_playButtonPushed();
    void slot_nextButtonPushed();
    void slot_previousButtonPushed();
    void slot_interruptingStopTriggered();

    void slot_playbackStateChanged ( QMediaPlayer::PlaybackState I_state );
    void slot_mediaStatusChanged ( QMediaPlayer::MediaStatus I_status );
    void slot_deviceChanged() const;
    // void slot_mediaPlayerError( QMediaPlayer::Error , const QString &I_qstr_error ) const;

    void slot_positionChanged( qint64 I_ms );

    void slot_setVolume( float I_linearVolume );

private:
    // First window snaps to the left/right/bottom/top of second window.
    enum class SnapType
    {
        NonSnap,
        ToLeft,
        ToRight,
        ToBottom,
        ToTop
    };

    void initializeConnection();

    void unsnapInvisibleWindows();
    SnapType checkSnapType      ( const QRect &I_geometry1, const QRect &I_geometry2 ) const;
    SnapType checkAdjacentType  ( const QRect &I_geometry1, const QRect &I_geometry2 ) const;
    bool breadthFirstSearch ( int I_idx ) const;

    void playItem ( QListWidgetItem *I_item );
    void playFile ( QListWidgetItem *I_item );

    QImage getCoverImage    ( TagLib::FLAC::File *I_flacFile );
    QImage getCoverImage    ( TagLib::Ogg::XiphComment *I_xiphComment );
    QImage getCoverImage    ( TagLib::ID3v2::Tag *I_id3v2Tag );
    QImage getCoverImage    ( TagLib::MP4::Tag *I_mp4Tag );

    TP_MainWindow       *   mainWindow;
    TP_PlaylistWindow   *   playlistWindow;
    TP_LyricsWindow     *   lyricsWindow;
    TP_ConfigWindow     *   configWindow;
    bool                    b_isPlaylistWindowVisible;
    bool                    b_isLyricsWindowVisible;

    QAudioOutput *  audioOutput;
    QMediaPlayer *  mediaPlayer;
    float           linearVolume;
    bool            b_isStopInterrupting;

    bool snapStatus [ TP::numberOfWindows ][ TP::numberOfWindows ];
    QPoint snapPosition_playlistWindow;
    QPoint snapPosition_lyricsWindow;
};

#endif // TP_MAINCLASS_H
