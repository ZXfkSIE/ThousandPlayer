#ifndef TP_MAINWINDOW_H
#define TP_MAINWINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class TP_MainWindow; }
QT_END_NAMESPACE

class TP_MainWindow : public QWidget
{
    Q_OBJECT

public:
    TP_MainWindow(QWidget *parent = nullptr);
    ~TP_MainWindow();

private slots:
    void on_pushButton_Exit_clicked();

private:
    Ui::TP_MainWindow *ui;
};
#endif // TP_MAINWINDOW_H
