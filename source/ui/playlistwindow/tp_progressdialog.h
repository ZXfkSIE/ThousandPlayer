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
};

#endif // TP_PROGRESSDIALOG_H
