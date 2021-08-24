#ifndef TP_MAINWINDOW_H
#define TP_MAINWINDOW_H

#include "tp_globalenum.h"

#include <QWidget>

namespace Ui { class TP_MainWindow; }

class TP_MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TP_MainWindow(QWidget *parent = nullptr);
    ~TP_MainWindow();

    void setCurrentAudioProperties(
            QString I_qstr_Format   = "N/A",
            int     bitDepth        = -1,
            int     sampleRate      = -1,
            int     bitRate         = -1,
            int     duration        = 0
            );

    void setPlay();
    void setPause();
    void setStop();

signals:
    void signal_openPlaylistWindow();
    void signal_hidePlaylistWindow();

    void signal_play();
    void signal_pause();
    void signal_stop();

public slots:
    void slot_playlistWindowShown();
    void slot_playlistWindowHidden();

    void slot_updateDuration(qint64 I_progress);

private slots:
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

    void setIcon_Play();
    void setIcon_Pause();

    void setIcon_Repeat();

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
