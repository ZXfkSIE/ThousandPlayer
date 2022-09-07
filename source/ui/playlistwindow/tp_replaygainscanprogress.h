#ifndef TP_REPLAYGAINSCANPROGRESS_H
#define TP_REPLAYGAINSCANPROGRESS_H

#include <QDialog>

namespace Ui { class TP_ReplayGainScanProgress; }

class QListWidgetItem;
class QThreadPool;

class TP_ReplayGainScanProgress : public QDialog
{
    Q_OBJECT

public:
    explicit TP_ReplayGainScanProgress( QWidget *parent = nullptr );
    ~TP_ReplayGainScanProgress();

    void addFile( QListWidgetItem *I_item );

    int exec() override;
    void showEvent( QShowEvent *event ) override;
    void reject() override;

private slots:
    void slot_onStart( unsigned I_index );
    void slot_onFinish( unsigned I_index, bool I_isSuccessful );

    void on_pushButton_clicked();

private:
    Ui::TP_ReplayGainScanProgress *ui;

    QThreadPool *threadPool;

    void daemon();

    bool b_isFinished;
    bool b_isAborted;
};

#endif // TP_REPLAYGAINSCANPROGRESS_H
