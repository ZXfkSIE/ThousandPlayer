/* ========== class TP_Button ==========
 * A derived QWidget class.
 * It passes all mouse move events to its parent
 * while preserving mouse press & release events.
 */

#ifndef TP_CONTAINER_H
#define TP_CONTAINER_H

#include <QWidget>

class TP_Container : public QWidget
{
    Q_OBJECT
public:
    explicit TP_Container(QWidget *parent = nullptr);

private:
    void mouseMoveEvent(QMouseEvent *event) override;

};

#endif // TP_CONTAINER_H
