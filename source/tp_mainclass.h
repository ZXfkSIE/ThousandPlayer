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

    void slot_moveWindow ( QWidget *window, QRect newGeometry );
    void slot_resizeWindow ( QWidget *window, QRect newGeometry, TP::ResizeType resizeType );
    void slot_refreshSnapStatus();

    void slot_connectFilelistWidget ( TP_FileListWidget* I_FilelistWidget );
    void slot_itemDoubleClicked ( QListWidgetItem *I_item );

    void slot_playButtonPushed();
    void slot_nextButtonPushed();
    void slot_previousButtonPushed();

    void slot_playbackStateChanged ( QMediaPlayer::PlaybackState newState );
    void slot_mediaStatusChanged ( QMediaPlayer::MediaStatus status );
    void slot_deviceChanged();

    void slot_changePlayingPosition( int second );

    void slot_setVolume( float linearVolume );

private:    
    void initializeConnection();

    void unsnapInvisibleWindows();
    TP::SnapType checkSnapType      ( const QRect &geometry1, const QRect &geometry2 ) const;
    TP::SnapType checkAdjacentType  ( const QRect &geometry1, const QRect &geometry2 ) const;
    bool breadthFirstSearch ( unsigned idx_Target ) const;

    void playItem ( QListWidgetItem *I_item );
    void playFile ( QListWidgetItem *I_item );

    QImage getCoverImageFromFLAC    ( const QString &filePath );
    QImage getCoverImageFromID3V2   ( const QString &filePath );

    TP_MainWindow       *   mainWindow;
    TP_PlaylistWindow   *   playlistWindow;
    TP_ConfigWindow     *   configWindow;
    bool                    b_isPlaylistWindowVisible;


    QAudioOutput *      audioOutput;
    QMediaPlayer *      mediaPlayer;
    float               linearVolume;

    bool snapStatus [ TP::numberOfWindows ][ TP::numberOfWindows ];
    QPoint snapPosition_playlistWindow;
};

#endif // TP_MAINCLASS_H
