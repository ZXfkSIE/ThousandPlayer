/* ========== class TP_Button ==========
 * A derived QFrame class.
 * It passes all mouse move events to its parent
 * while preserving mouse press & release events.
 */

#ifndef TP_FRAME_H
#define TP_FRAME_H

#include <QFrame>

class TP_Frame : public QFrame
{
    Q_OBJECT

public:
    explicit TP_Frame(QWidget *parent = nullptr);

private:
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // TP_FRAME_H
