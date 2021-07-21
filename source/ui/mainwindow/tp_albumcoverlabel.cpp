#include "tp_albumcoverlabel.h"

#include <QIcon>

TP_AlbumCoverLabel::TP_AlbumCoverLabel(QWidget *parent) :
    TP_Label(parent)
{
    setAlignment(Qt::AlignCenter);
    setMaximumSize( QSize{128, 128} );
    setMinimumSize( QSize{128, 128} );
}

void
TP_AlbumCoverLabel::setImage()
{
    setPixmap( QIcon(":/image/MusicalNote.svg")
               .pixmap(QSize(96, 96)) );
}


// *****************************************************************
// public override
// *****************************************************************

QSize
TP_AlbumCoverLabel::sizeHint() const
{
    return QSize(128, 128);
}
