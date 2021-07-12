#ifndef TP_TITLEBAR_H
#define TP_TITLEBAR_H

#include <QFrame>

class QMouseEvent;

class TP_TitleBar : public QFrame
{

    Q_OBJECT
public:
    TP_TitleBar(QWidget *parent = nullptr);

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    bool isBeingPressed;
    QPoint pressedPosition;
};

#endif // TP_TITLEBAR_H
