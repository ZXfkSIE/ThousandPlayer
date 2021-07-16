#ifndef TP_TITLEBAR_H
#define TP_TITLEBAR_H

#include <QFrame>

class QMouseEvent;

class TP_TitleBar : public QFrame
{

    Q_OBJECT

public:
    explicit TP_TitleBar(QWidget *parent = nullptr);

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    QPoint pressedRelativePosition;
};

#endif // TP_TITLEBAR_H
