#ifndef TP_CONFIGWINDOW_H
#define TP_CONFIGWINDOW_H

#include <QAudioDevice>
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

private slots:
    void slot_switchPage( const int currentRow );

    void slot_audioDeviceChanged( int index );
    void slot_ReplayGainModeChanged( int index );
    void slot_setPreAmp( int value );
    void slot_setDefaultReplayGain( int value );

private:
    Ui::TP_ConfigWindow *ui;

    void initializeConnection();
    void initializeUI();
};

#endif // TP_CONFIGWINDOW_H
