#ifndef TP_CONFIGWINDOW_H
#define TP_CONFIGWINDOW_H

#include <QDialog>

namespace Ui { class TP_ConfigWindow; }

class TP_ConfigWindow : public QDialog
{
    Q_OBJECT

public:
    explicit TP_ConfigWindow(QWidget *parent = nullptr);
    ~TP_ConfigWindow();

private:
    Ui::TP_ConfigWindow *ui;
};

#endif // TP_CONFIGWINDOW_H
