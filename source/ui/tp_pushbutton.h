/* =============== class TP_Menu ===============
 * A derived QPushButton class which
 * omits the menu indicator "▲" and
 * change the style of tooltips.
 */

#ifndef TP_PUSHBUTTON_H
#define TP_PUSHBUTTON_H

#include <QPushButton>

class TP_PushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit TP_PushButton( QWidget *parent = nullptr );
};

#endif // TP_PUSHBUTTON_H
