﻿#ifndef TP_MAINWINDOW_H
#define TP_MAINWINDOW_H

#include "tp_globalenum.h"

#include <QWidget>

class TP_Menu;
class QListWidgetItem;

namespace Ui { class TP_MainWindow; }

class TP_MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TP_MainWindow( QWidget *parent = nullptr );
    ~TP_MainWindow();

    void initializeVolume();

    void setPlay();
    void setPause();
    void setStop();

    void setAudioInformation( QListWidgetItem *I_item );
    void setFileNotFound();

    void setCover( const QImage &I_image );

signals:
    void signal_minimizeWindow();
    void signal_restoreWindow();
    void signal_activateWindow();
    void signal_moveWindow( QWidget *window, QRect newGeometry );
    void signal_leftButtonReleased();

    void signal_resizeWindow( QWidget *window, QRect newGeometry, TP::ResizeType resizeType );

    void signal_timeSliderPressed( int second );
    void signal_volumeSliderValueChanged( float logarithmicVolume );

    void signal_openPlaylistWindow();
    void signal_hidePlaylistWindow();
    void signal_openConfigWindow();

    void signal_playButtonPushed();
    void signal_pauseButtonPushed();
    void signal_stopButtonPushed();
    void signal_nextButtonPushed();
    void signal_previousButtonPushed();

    void signal_modeIsNotShuffle();

public slots:
    void slot_playlistWindowShown();
    void slot_playlistWindowHidden();

    void slot_updateDuration( qint64 ms );

private slots:
    void slot_moveTitleBar( const QRect &newGeometry );
    void slot_leftButtonReleased();

    void slot_timeSliderChanged( int second );
    void slot_timeSliderPressed( int second );

    void slot_volumeSliderChanged( int volume );

    void on_pushButton_Exit_clicked() const;
    void on_pushButton_Expand_clicked();
    void on_pushButton_Minimize_clicked();

    void on_pushButton_Playlist_clicked();
    void on_pushButton_Config_clicked();

    void on_pushButton_Play_clicked();
    void on_pushButton_Stop_clicked();
    void on_pushButton_Next_clicked();
    void on_pushButton_Previous_clicked();

    void on_action_setMode_SingleTime_triggered();
    void on_action_setMode_Repeat_triggered();
    void on_action_setMode_Sequential_triggered();
    void on_action_setMode_Shuffle_triggered();

private:
    Ui::TP_MainWindow *ui;

    void changeEvent( QEvent *event ) override;

    void mousePressEvent( QMouseEvent *event ) override;
    void mouseMoveEvent( QMouseEvent *event ) override;
    void mouseReleaseEvent( QMouseEvent *event ) override;

    void initializeConnection();
    void initializeMenu();
    void initializeUI();

    void setIcon_Play();
    void setIcon_Pause();

    void setAudioInfoLabel( QListWidgetItem *I_item = nullptr );
    void setAudioPropertyLabels(
            const QString & I_qstr_Format   = QString { "N/A" },
            int             bitDepth        = -1,
            int             sampleRate      = -1,
            int             bitRate         = -1,
            int             duration        = 0
            );

    QString convertTime( qint64 second ) const;

    TP::CursorPositionType isAtBorder( const QPoint &I_point ) const;

    bool b_isBorderBeingPressed;
    bool b_isCursorResize;

    bool b_isPlaylistWindowShown;

    TP_Menu *menu_Mode;

    TP::CursorPositionType cursorPositionType;
};
#endif // TP_MAINWINDOW_H
