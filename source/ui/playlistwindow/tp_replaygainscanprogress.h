#ifndef TP_REPLAYGAINSCANPROGRESS_H
#define TP_REPLAYGAINSCANPROGRESS_H

#include <QDialog>

namespace Ui { class TP_ReplayGainScanProgress; }

class QTableWidgetItem;

class TP_ReplayGainScanProgress : public QDialog
{
    Q_OBJECT

public:
    explicit TP_ReplayGainScanProgress( QWidget *parent = nullptr );
    ~TP_ReplayGainScanProgress();

    void addFile( QTableWidgetItem *I_item );
    void startScanning();

private:
    Ui::TP_ReplayGainScanProgress *ui;
};

#endif // TP_REPLAYGAINSCANPROGRESS_H
