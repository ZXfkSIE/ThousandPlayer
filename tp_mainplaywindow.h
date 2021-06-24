#ifndef TP_MAINPLAYWINDOW_H
#define TP_MAINPLAYWINDOW_H

#include <QWidget>

class QGraphicsOpacityEffect;
class QPropertyAnimation;

QT_BEGIN_NAMESPACE
namespace Ui { class TP_MainPlayWindow; }
QT_END_NAMESPACE

class TP_MainPlayWindow : public QWidget
{
    Q_OBJECT

public:
    TP_MainPlayWindow(QWidget *parent = nullptr);
    ~TP_MainPlayWindow();

    void show();

private:
    Ui::TP_MainPlayWindow *ui;
};
#endif // TP_MAINPLAYWINDOW_H
