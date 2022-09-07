/* ========== class TP_ProgressDialog ==========
 * A derived QProgressDialog class.
 * Has a customized appearance.
 */

#ifndef TP_PROGRESSDIALOG_H
#define TP_PROGRESSDIALOG_H

#include <QProgressDialog>

class TP_ProgressDialog : public QProgressDialog
{
    Q_OBJECT

public:
    TP_ProgressDialog(
            const QString &labelText,
            const QString &cancelButtonText,
            int minimum,
            int maximum,
            QWidget *parent = nullptr,
            Qt::WindowFlags f = Qt::WindowFlags()
            );

    void initialize( int I_max, int I_idx_min, int I_idx_max );

signals:
    void signal_onComplete( int I_idx_min, int I_idx_max );

public slots:
    void slot_addCount();

private:
    static const int percentageStep { 1 };

    int nextPercentage;
    unsigned n_finished;
    int idx_min;
    int idx_max;
};

#endif // TP_PROGRESSDIALOG_H
