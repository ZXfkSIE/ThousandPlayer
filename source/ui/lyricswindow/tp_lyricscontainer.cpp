#include "tp_lyricscontainer.h"

#include <QPainter>
#include <QStyleOption>

TP_LyricsContainer::TP_LyricsContainer( QWidget *parent ) :
    QWidget { parent }
{
    setStyleSheet(
"TP_LyricsContainer"
"{"
    "border-bottom: 5px solid #666;"
    "border-left: 5px solid #666;"
    "border-right: 4px solid rgb(68, 68, 68);"
"}"
                );
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_LyricsContainer::paintEvent( QPaintEvent * )
{
    QStyleOption opt;
    opt.initFrom( this );
    QPainter p { this };
    style()->drawPrimitive( QStyle::PE_Widget, &opt, &p, this );
}
