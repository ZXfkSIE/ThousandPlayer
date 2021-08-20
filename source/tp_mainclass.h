#ifndef TP_MAINCLASS_H
#define TP_MAINCLASS_H

#include <QObject>

class TP_MainWindow;
class TP_PlaylistWindow;
class TP_FileListWidget;

class QAudioOutput;
class QListWidgetItem;
class QMediaPlayer;

class TP_MainClass : public QObject
{
    Q_OBJECT

public:
    explicit TP_MainClass();
    ~TP_MainClass();

public slots:
    void slot_checkIfServiceAvailable();
    void slot_initializePosition();
    void slot_connectFilelistWidget( TP_FileListWidget* I_FilelistWidget );
    void slot_playURL( QListWidgetItem *I_listWidgetItem );

private slots:
    void slot_hasAudioChanged(bool isAvailable);

private:
    void initializeConnection();

    TP_MainWindow       *mainWindow;
    TP_PlaylistWindow   *playlistWindow;

    QAudioOutput        *audioOutput;
    QMediaPlayer        *mediaPlayer;
};

#endif // TP_MAINCLASS_H
