#ifndef TP_MAINCLASS_H
#define TP_MAINCLASS_H

#include "tp_globalenum.h"

#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QObject>

#include <array>

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
    void slot_playItem( QListWidgetItem *I_listWidgetItem );
    void slot_moveWindow( QWidget *window, QRect newGeometry );
    void slot_leftButtonReleased();
    void slot_resizeWindow( QWidget *window, QRect newGeometry, TP::ResizeType resizeType );
    void slot_connectFilelistWidget( TP_FileListWidget* I_FilelistWidget );
    void slot_playbackStateChanged( QMediaPlayer::PlaybackState newState );
    void slot_playButtonPushed();
    void slot_changePlayingPosition( int second );

private:    
    void initializeConnection();

    void unsnapInvisibleWindows();
    TP::SnapType checkSnapType( const QRect &geometry1, const QRect &geometry2) const;
    TP::SnapType checkAdjacentType( const QRect &geometry1, const QRect &geometry2) const;
    bool breadthFirstSearch( unsigned idx_Target ) const;

    void playFile( QListWidgetItem *I_listWidgetItem );

    TP_MainWindow       *mainWindow;
    TP_PlaylistWindow   *playlistWindow;

    QAudioOutput        *audioOutput;
    QMediaPlayer        *mediaPlayer;
    QListWidgetItem     currentItem;

    bool snapStatus [4] [4];
    QPoint snapPosition_playlistWindow;
};

#endif // TP_MAINCLASS_H
