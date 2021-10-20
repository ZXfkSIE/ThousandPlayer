#ifndef TP_CONFIGWINDOW_H
#define TP_CONFIGWINDOW_H

#include <QDialog>

namespace Ui { class TP_ConfigWindow; }

class TP_ConfigWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TP_ConfigWindow( QWidget *parent = nullptr );
    ~TP_ConfigWindow();

private slots:
    void slot_audioDeviceChanged();
    void slot_ReplayGainModeChanged();

private:
    Ui::TP_ConfigWindow *ui;

    void initializeConnection();
    void initializeUI();
};

#endif // TP_CONFIGWINDOW_H
