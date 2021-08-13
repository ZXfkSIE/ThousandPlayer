#include "tp_albumcoverlabel.h"

#include <QIcon>

TP_AlbumCoverLabel::TP_AlbumCoverLabel(QWidget *parent, int I_size) :
    QLabel(parent),
    size(I_size)
{
    setAlignment(Qt::AlignCenter);
    setMaximumSize( QSize{size, size} );
    setMinimumSize( QSize{size, size} );
}

// Default musical note image
void
TP_AlbumCoverLabel::setImage()
{
    int noteSize = size * 3 / 4;
    setPixmap( QIcon(":/image/MusicalNote.svg")
               .pixmap(QSize(noteSize, noteSize)) );
}

void
TP_AlbumCoverLabel::setImage(const QPixmap &I_pixmap)
{
    setPixmap( I_pixmap.scaled(
                   QSize(size, size),
                   Qt::KeepAspectRatio,
                   Qt::SmoothTransformation
                   )
               );
}

// *****************************************************************
// public override
// *****************************************************************

QSize
TP_AlbumCoverLabel::sizeHint() const
{
    return QSize(size, size);
}
