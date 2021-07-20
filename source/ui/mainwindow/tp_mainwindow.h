#ifndef TP_MAINWINDOW_H
#define TP_MAINWINDOW_H

#include <QWidget>

namespace Ui { class TP_MainWindow; }

class TP_MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TP_MainWindow(QWidget *parent = nullptr);
    ~TP_MainWindow();

private slots:
    void on_pushButton_Exit_clicked();

private:
    Ui::TP_MainWindow *ui;

    static const int TP_LEFT_BORDER = -1, TP_RIGHT_BORDER = 1;
    static const int TP_BORDER_SIZE = 5;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    int isAtBorder(QPoint I_qpt);

    bool b_isBorderBeingPressed;
    bool b_isCursorResize;
    bool b_isExpandingDisabled;
    int cursorPositionType;
    QPoint pressedGlobalPosition;
};
#endif // TP_MAINWINDOW_H
