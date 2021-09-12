#include "tp_coverlabel.h"

#include "tp_coverviewer.h"
#include "tp_menu.h"

#include <QAction>
#include <QIcon>
#include <QMouseEvent>

TP_CoverLabel::TP_CoverLabel( QWidget *parent ) :
    QLabel          { parent }
  , coverViewer     { new TP_CoverViewer { nullptr } }
{
    initializeMenu();

    setAlignment( Qt::AlignCenter );
}

TP_CoverLabel::~TP_CoverLabel()
{
    coverViewer->close();
    delete coverViewer;
}

// Default musical note image
void
TP_CoverLabel::setImage()
{
    act_viewCoverImage->setEnabled( false );

    coverViewer->close();
    coverViewer->setImage( {} );

    int noteSize = width() * 3 / 4;
    setPixmap( QIcon(":/image/MusicalNote.svg")
               .pixmap( QSize( noteSize, noteSize ) )
               );
}

void
TP_CoverLabel::setImage( const QPixmap &I_pixmap )
{
    act_viewCoverImage->setEnabled( true );
    coverViewer->close();
    coverViewer->setImage( I_pixmap );

    setPixmap( I_pixmap.scaled(
                   QSize(width() - 8, width() - 8),
                   Qt::KeepAspectRatio,
                   Qt::SmoothTransformation
                   )
               );
}


// *****************************************************************
// private slots
// *****************************************************************


void
TP_CoverLabel::slot_viewCoverImage()
{
    if( act_viewCoverImage->isEnabled() )
    {
        if( coverViewer->isVisible() )
            coverViewer->close();

        coverViewer->show();
    }
}


// *****************************************************************
// private override
// *****************************************************************


void
TP_CoverLabel::mouseDoubleClickEvent( QMouseEvent *event )
{
    if( event->button() == Qt::LeftButton
            && act_viewCoverImage->isEnabled() )
        slot_viewCoverImage();
    else
        event->ignore();
}


void
TP_CoverLabel::contextMenuEvent( QContextMenuEvent *event )
{
    menu_rightClick->exec( event->globalPos() );
}


// *****************************************************************
// private
// *****************************************************************


void
TP_CoverLabel::initializeMenu()
{
    act_viewCoverImage = new QAction { tr("&View cover"), this };

    connect(act_viewCoverImage, &QAction::triggered,
            this,               &TP_CoverLabel::slot_viewCoverImage);

    menu_rightClick = new TP_Menu { this };
    menu_rightClick->addAction( act_viewCoverImage );

    act_viewCoverImage->setEnabled( false );
}
