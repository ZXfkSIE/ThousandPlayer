/* ========== class TP_Label ==========
 * A derived QLabel class.
 * It passes all mouse move events to its parent
 * while preserving mouse press & release events.
 */

#ifndef TP_LABEL_H
#define TP_LABEL_H

#include <QLabel>

class TP_Label : public QLabel
{
    Q_OBJECT
public:
    explicit TP_Label(QWidget *parent = nullptr);

private:
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // TP_LABEL_H
