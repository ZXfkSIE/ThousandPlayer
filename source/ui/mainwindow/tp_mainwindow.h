#ifndef TP_MAINWINDOW_H
#define TP_MAINWINDOW_H

#include "tp_globalenum.h"

#include <QWidget>

class QListWidgetItem;

namespace Ui { class TP_MainWindow; }

class TP_MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TP_MainWindow(QWidget *parent = nullptr);
    ~TP_MainWindow();

    void setPlay();
    void setPause();
    void setStop();

    void setAudioInformation(const QListWidgetItem &I_listWidgetItem);
    void setFileNotFound();

    void setVolumeSliderValue(int value);

signals:
    void signal_moveWindow( QWidget *window, QRect newGeometry );
    void signal_titleBarReleased();

    void signal_resizeWindow( QWidget *window, QRect newGeometry, TP::ResizeType resizeType );

    void signal_timeSliderPressed( int second );
    void signal_volumeSliderValueChanged( float logarithmicVolume );

    void signal_openPlaylistWindow();
    void signal_hidePlaylistWindow();

    void signal_playButtonPushed();
    void signal_pauseButtonPushed();
    void signal_stopButtonPushed();

public slots:
    void slot_changeVolumeSlider(float volume);

    void slot_playlistWindowShown();
    void slot_playlistWindowHidden();

    void slot_updateDuration(qint64 ms);

private slots:
    void slot_moveTitleBar( QRect newGeometry );
    void slot_titleBarReleased();

    void slot_timeSliderChanged( int second );
    void slot_timeSliderPressed( int second );

    void slot_volumeSliderChanged( int volume );

    void on_pushButton_Exit_clicked() const;
    void on_pushButton_Expand_clicked();

    void on_pushButton_Playlist_clicked();

    void on_pushButton_Play_clicked();
    void on_pushButton_Stop_clicked();

private:
    Ui::TP_MainWindow *ui;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void initializeConnection();

    void setIcon_Play();
    void setIcon_Pause();

    void setIcon_Repeat();

    void setAudioPropertyLabels(
            QString I_qstr_Format   = "N/A",
            int     bitDepth        = -1,
            int     sampleRate      = -1,
            int     bitRate         = -1,
            int     duration        = 0
            );

    QString convertTime(qint64 second) const;

    TP::CursorPositionType isAtBorder(QPoint I_point) const;

    bool b_isBorderBeingPressed;
    bool b_isCursorResize;
    bool b_isExpandingDisabled;

    bool b_isPlaylistWindowShown;
    bool b_isPlaying;

    TP::CursorPositionType  cursorPositionType;
    QPoint                  pressedGlobalPosition;
};
#endif // TP_MAINWINDOW_H
