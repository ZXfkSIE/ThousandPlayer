/* =============== class TP_TitleBar ===============
 * A derived QFrame class.
 * Acts like a system title bar.
 * User can push and drag it to move the window
 * to which it belongs.
 * Pass mouse move events to its parent while not
 * being pressed.
 */

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
    void mouseReleaseEvent(QMouseEvent *event) override;

    bool b_isBeingPressed;
    QPoint pressedRelativePosition;
};

#endif // TP_TITLEBAR_H
