#ifndef TP_MAINCLASS_H
#define TP_MAINCLASS_H

#include <QListWidgetItem>
#include <QMediaPlayer>
#include <QObject>

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
    void slot_playItem( QListWidgetItem *I_listWidgetItem );
    void slot_moveWindow( QWidget *window, QRect newGeometry );

private slots:
    void slot_connectFilelistWidget( TP_FileListWidget* I_FilelistWidget );
    void slot_playbackStateChanged( QMediaPlayer::PlaybackState newState );

    void slot_playButtonPushed();

private:
    void initializeConnection();

    void playFile( QListWidgetItem *I_listWidgetItem );

    TP_MainWindow       *mainWindow;
    TP_PlaylistWindow   *playlistWindow;

    QAudioOutput        *audioOutput;
    QMediaPlayer        *mediaPlayer;

    QListWidgetItem     currentItem;
};

#endif // TP_MAINCLASS_H
