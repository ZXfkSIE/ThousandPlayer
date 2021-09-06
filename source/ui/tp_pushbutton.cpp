#include "tp_pushbutton.h"

TP_PushButton::TP_PushButton( QWidget *parent ) :
    QPushButton { parent }
{
    setStyleSheet(
                "QPushButton { color: rgb(255, 255, 255); }"
                "QPushButton::menu-indicator { image:none; width:0px; }"
                );
}
