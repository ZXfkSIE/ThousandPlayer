﻿/* ========================= class TP_TitleBar =========================
 * A derived QFrame class which acts like a system title bar.
 * User can push and drag it to move the window to which it belongs.
 * Pass mouse move events to its parents while not being pressed.
 */

#ifndef TP_TITLEBAR_H
#define TP_TITLEBAR_H

#include <QFrame>

class QMouseEvent;

class TP_TitleBar : public QFrame
{

    Q_OBJECT

public:
    explicit TP_TitleBar( QWidget *parent = nullptr );

signals:
    void signal_moveTitleBar( QRect newGeometry );
    void signal_leftButtonReleased();

private:
    void mousePressEvent( QMouseEvent *event ) override;
    void mouseMoveEvent( QMouseEvent *event ) override;
    void mouseReleaseEvent( QMouseEvent *event ) override;

    bool b_isBeingPressed;
    QPoint pressedRelativePosition;
};

#endif // TP_TITLEBAR_H
