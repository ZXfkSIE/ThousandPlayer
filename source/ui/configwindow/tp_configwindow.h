#ifndef TP_CONFIGWINDOW_H
#define TP_CONFIGWINDOW_H

#include <QAudioDevice>
#include <QFont>
#include <QDialog>

namespace Ui { class TP_ConfigWindow; }

class TP_ConfigWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TP_ConfigWindow( QWidget *parent = nullptr );
    ~TP_ConfigWindow();

signals:
    void signal_audioDeviceChanged( const QAudioDevice &device );

    void signal_audioInfoLabelFontChanged();
    void signal_playlistFontChanged();
    void signal_lyricsFontChanged();

private slots:
    void on_listWidget_Tab_currentRowChanged( int I_row );

    void on_radioButton_ClickX_MinimizeToTray_toggled( bool I_checked );
    void on_pushButton_ChangeAudioInfoLabelFont_clicked();
    void on_spinBox_AudioInfoLabelScrollingInterval_valueChanged( int I_sec );

    void on_comboBox_AudioDevice_currentIndexChanged( int I_idx );
    void on_comboBox_ReplayGainMode_currentIndexChanged ( int I_idx );
    void on_slider_PreAmp_valueChanged( int I_value );
    void on_slider_DefaultReplayGain_valueChanged ( int I_value );
    void on_lineEdit_RsgainPath_textChanged( const QString & );
    void on_pushButton_RsgainPath_clicked();
    void on_checkBox_isExistingReplayGainSkipped_stateChanged( int newState );

    void on_pushButton_ChangePlaylistFont_clicked();

    void on_pushButton_ChangeLyricsFont_clicked();
    void on_spinBox_JumpingTimeOffset_valueChanged( int I_ms );

    void on_pushButton_GPL_clicked();
    void on_pushButton_Credits_clicked();

    void on_buttonBox_OK_accepted();

private:
    Ui::TP_ConfigWindow *ui;

    void initializeStrings();
    void initializeUI();

    QString qstr_GPL;
    QString qstr_Credits;
};

#endif // TP_CONFIGWINDOW_H
