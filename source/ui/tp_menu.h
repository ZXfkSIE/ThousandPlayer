/* =============== class TP_Menu ===============
 * A derived QMenu class.
 * Has a customized appearance.
 */

#ifndef TP_MENU_H
#define TP_MENU_H

#include <QMenu>

class TP_Menu : public QMenu
{
    Q_OBJECT

public:
    explicit TP_Menu( QWidget *parent = nullptr );
};

#endif // TP_MENU_H
