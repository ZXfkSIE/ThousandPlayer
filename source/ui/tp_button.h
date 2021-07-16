/* ========== class TP_Button ==========
 * A derived QPushButton class.
 * It passes all mouse move events to its parent
 * while preserving mouse press & release events.
 */

#ifndef TP_BUTTON_H
#define TP_BUTTON_H

#include <QPushButton>

class TP_Button : public QPushButton
{
    Q_OBJECT

public:
    explicit TP_Button(QWidget *parent = nullptr);

private:
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // TP_BUTTON_H
