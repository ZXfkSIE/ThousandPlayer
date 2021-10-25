#include "tp_pushbutton.h"

TP_PushButton::TP_PushButton( QWidget *parent ) :
    QPushButton { parent }
{
    setStyleSheet(
                "QPushButton::menu-indicator { image:none; width:0px; }"
                "QToolTip { color: black; background-color: #fafafa; border: 0px; }"
                );
}
