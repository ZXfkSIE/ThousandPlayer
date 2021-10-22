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
    void signal_audioDeviceChanged( QAudioDevice device );

    void signal_fontChanged();

private slots:
    void slot_switchPage( const int currentRow );

    void slot_audioDeviceChanged( const int index );
    void slot_ReplayGainModeChanged( const int index );
    void slot_setPreAmp( const int value );
    void slot_setDefaultReplayGain( const int value );

    void on_pushButton_ChangePlaylistFont_clicked();

private:
    Ui::TP_ConfigWindow *ui;

    void initializeConnection();
    void initializeUI();
};

#endif // TP_CONFIGWINDOW_H
