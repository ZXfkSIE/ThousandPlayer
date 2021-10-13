#ifndef TP_FILESEARCHDIALOG_H
#define TP_FILESEARCHDIALOG_H

#include <QDialog>

namespace Ui { class TP_FileSearchDialog; }

class TP_FileSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TP_FileSearchDialog( QWidget *parent = nullptr );
    ~TP_FileSearchDialog();

private:
    Ui::TP_FileSearchDialog *ui;
};

#endif // TP_FILESEARCHDIALOG_H
