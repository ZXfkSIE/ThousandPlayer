#ifndef TP_MAINWINDOW_H
#define TP_MAINWINDOW_H

#include "tp_globalenum.h"

#include <QWidget>
#include <QSystemTrayIcon>

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

    void updatePosition( qint64 I_ms );

    void setAudioInfomation( const QListWidgetItem *I_item = nullptr );
    void setAudioInfoLabelTexts( std::vector< QString > &&I_vec_Qstr );
    void setFileNotFound();

    void setCover( const QImage &I_image );

signals:
    void signal_exitProgram();
    void signal_minimizeWindow();
    void signal_restoreWindow();
    void signal_activateWindow();

    void signal_moveWindow( QWidget *window, const QRect &geometry );
    void signal_resizeWindow( QWidget *window, const QRect &geometry, TP::CursorPositionType resizeType );
    void signal_windowChanged();

    void signal_timeSliderPressed( qint64 ms );
    void signal_volumeSliderValueChanged( float logarithmicVolume );

    void signal_openPlaylistWindow();
    void signal_hidePlaylistWindow();
    void signal_openLyricsWindow();
    void signal_hideLyricsWindow();
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
    void slot_lyricsWindowShown();
    void slot_lyricsWindowHidden();

    void slot_changeFontOfAudioInfoLabel();

private slots:
    void slot_trayIcon_activated( QSystemTrayIcon::ActivationReason I_reason );
    void on_action_trayIcon_Restore_triggered();
    void on_action_trayIcon_Exit_triggered();

    void slot_titleBarMoved( const QRect &I_geometry );
    void slot_leftButtonReleased();

    void on_slider_Time_valueChanged( int I_second );
    void slot_timeSliderPressed( int I_second );

    void on_slider_Volume_valueChanged( int I_volume );

    void on_pushButton_Exit_clicked();
    void on_pushButton_Expand_clicked();
    void on_pushButton_Minimize_clicked();

    void on_pushButton_Playlist_clicked();
    void on_pushButton_Lyrics_clicked();
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

    void mousePressEvent    ( QMouseEvent *event ) override;
    void mouseMoveEvent     ( QMouseEvent *event ) override;
    void mouseReleaseEvent  ( QMouseEvent *event ) override;

    void initializeConnection();
    void initializeMenu();
    void initializeUI();

    void setIcon_Play();
    void setIcon_Pause();

    void setAudioInfoLabel( const QListWidgetItem *I_item = nullptr );
    void setAudioPropertyLabels(
            const QString & I_qstr_format   = QString { "N/A" },
            int             I_bitDepth      = -1,
            int             I_sampleRate    = -1,
            int             I_bitRate       = -1,
            int             I_duration      = 0,
            float           I_replayGain    = 0
            );


    QString convertTime( qint64 I_second ) const;
    QString toString_AudioFormat( TP::AudioFormat I_type ) const;

    bool b_isBorderBeingPressed;
    bool b_isCursorResize;

    bool b_isPlaylistWindowShown;
    bool b_isLyricsWindowShown;

    TP_Menu         *   menu_Mode;

    QSystemTrayIcon *   trayIcon;
    QMenu           *   menu_trayIcon;

    TP::CursorPositionType cursorPositionType;
};
#endif // TP_MAINWINDOW_H
