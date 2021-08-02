#include "tp_menu.h"

TP_Menu::TP_Menu(QWidget *parent) :
    QMenu { parent }
{
    QFont tempFont = font();
    tempFont.setPointSize(10);
    setFont(tempFont);
    setStyleSheet(
"QMenu"
"{"
"color: rgb(255, 255, 255);"
"}"
"QMenu::item"
"{"
"background-color: transparent;"
"}"
"QMenu::item:selected"
"{"
"background-color: rgb(0, 0, 0);"
"}");
}
