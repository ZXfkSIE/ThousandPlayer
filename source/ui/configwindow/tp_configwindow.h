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
    explicit TP_ConfigWindow ( QWidget *parent = nullptr );
    ~TP_ConfigWindow ();

signals:
    void signal_audioDeviceChanged ( QAudioDevice device );

    void signal_audioInfoLabelFontChanged ();
    void signal_playlistFontChanged ();

private slots:
    void on_listWidget_Tab_currentRowChanged( const int currentRow );

    void on_radioButton_ClickX_MinimizeToTray_toggled( const bool checked );
    void on_pushButton_ChangeAudioInfoLabelFont_clicked();
    void on_spinBox_AudioInfoLabelScrollingInterval_valueChanged( const int I_sec );

    void on_comboBox_AudioDevice_currentIndexChanged( const int index );
    void on_comboBox_ReplayGainMode_currentIndexChanged ( const int index );
    void on_slider_PreAmp_valueChanged ( const int value );
    void on_slider_DefaultReplayGain_valueChanged ( const int value );

    void on_pushButton_ChangePlaylistFont_clicked ();

    void on_pushButton_GPL_clicked ();
    void on_pushButton_Credits_clicked ();

private:
    Ui::TP_ConfigWindow *ui;

    void initializeStrings ();
    void initializeUI ();

    std::string str_GPL;
    std::string str_Credits;
};

#endif // TP_CONFIGWINDOW_H
