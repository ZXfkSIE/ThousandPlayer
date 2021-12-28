#ifndef TP_MAINCLASS_H
#define TP_MAINCLASS_H

#include "tp_globalconst.h"
#include "tp_globalenum.h"

#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QObject>

#include <array>

class TP_ConfigWindow;
class TP_MainWindow;
class TP_PlaylistWindow;
class TP_FileListWidget;

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
    void slot_minimizeWindow();
    void slot_restoreWindow();

    void slot_moveWindow ( QWidget *I_window, QRect I_geometry );
    void slot_resizeWindow ( QWidget *I_window, QRect I_geometry, TP::ResizeType I_resizeType );
    void slot_refreshSnapStatus();

    void slot_itemDoubleClicked ( QListWidgetItem *I_item );

    void slot_playButtonPushed();
    void slot_nextButtonPushed();
    void slot_previousButtonPushed();
    void slot_interruptingStopTriggered();

    void slot_playbackStateChanged ( QMediaPlayer::PlaybackState newState );
    void slot_mediaStatusChanged ( QMediaPlayer::MediaStatus status );
    void slot_deviceChanged() const;
    void slot_mediaPlayerError( QMediaPlayer::Error error, const QString &errorString ) const;

    void slot_positionChanged( qint64 I_ms );
    void slot_changePlayingPosition( int I_second );

    void slot_setVolume( float I_linearVolume );

private:    
    void initializeConnection();

    void unsnapInvisibleWindows();
    TP::SnapType checkSnapType      ( const QRect &geometry1, const QRect &geometry2 ) const;
    TP::SnapType checkAdjacentType  ( const QRect &geometry1, const QRect &geometry2 ) const;
    bool breadthFirstSearch ( unsigned idx_Target ) const;

    void playItem ( QListWidgetItem *I_item );
    void playFile ( QListWidgetItem *I_item );

    QImage getCoverImage    ( TagLib::FLAC::File *flacFile );
    QImage getCoverImage    ( TagLib::Ogg::XiphComment *xiphComment );
    QImage getCoverImage    ( TagLib::ID3v2::Tag *tag );
    QImage getCoverImage    ( TagLib::MP4::Tag *tag );

    TP_MainWindow       *   mainWindow;
    TP_PlaylistWindow   *   playlistWindow;
    TP_ConfigWindow     *   configWindow;
    bool                    b_isPlaylistWindowVisible;

    QAudioOutput *  audioOutput;
    QMediaPlayer *  mediaPlayer;
    float           linearVolume;
    bool            b_isStopInterrupting;

    bool snapStatus [ TP::numberOfWindows ][ TP::numberOfWindows ];
    QPoint snapPosition_playlistWindow;
};

#endif // TP_MAINCLASS_H
