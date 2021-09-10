#include "tp_albumcoverlabel.h"

#include "tp_menu.h"

#include <QAction>
#include <QIcon>
#include <QMouseEvent>

TP_AlbumCoverLabel::TP_AlbumCoverLabel( QWidget *parent ) :
    QLabel { parent }
  , currentPixmap {}
{
    setAlignment( Qt::AlignCenter );
}

// Default musical note image
void
TP_AlbumCoverLabel::setImage()
{
    int noteSize = width() * 3 / 4;
    setPixmap( QIcon(":/image/MusicalNote.svg")
               .pixmap(QSize(noteSize, noteSize)) );
}

void
TP_AlbumCoverLabel::setImage(const QPixmap &I_pixmap)
{
    currentPixmap = I_pixmap;

    setPixmap( I_pixmap.scaled(
                   QSize(width(), width()),
                   Qt::KeepAspectRatio,
                   Qt::SmoothTransformation
                   )
               );
}


// *****************************************************************
// private slots
// *****************************************************************


void
TP_AlbumCoverLabel::slot_viewCoverImage()
{

}


// *****************************************************************
// private override
// *****************************************************************


void
TP_AlbumCoverLabel::mouseDoubleClickEvent( QMouseEvent *event )
{
    if( event->button() == Qt::LeftButton )
        slot_viewCoverImage();
    else
        event->ignore();
}


void
TP_AlbumCoverLabel::contextMenuEvent( QContextMenuEvent *event )
{
    menu_rightClick->exec( event->globalPos() );
}


// *****************************************************************
// private
// *****************************************************************


void
TP_AlbumCoverLabel::initializeMenu()
{
    act_viewCoverImage = new QAction { tr("&View cover image..."), this };

    connect(act_viewCoverImage, &QAction::triggered,
            this,               &TP_AlbumCoverLabel::slot_viewCoverImage);

    menu_rightClick = new TP_Menu { this };
    menu_rightClick->addAction( act_viewCoverImage );
}
