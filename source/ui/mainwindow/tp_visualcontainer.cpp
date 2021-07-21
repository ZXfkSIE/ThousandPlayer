#include "tp_visualcontainer.h"

#include "tp_globalconst.h"
#include "tp_albumcoverlabel.h"

#include <QHBoxLayout>

TP_VisualContainer::TP_VisualContainer(QWidget *parent) :
    TP_Container( parent )
  , layout( new QHBoxLayout{this} )
  , tp_AlbumCoverLabel( new TP_AlbumCoverLabel{this} )
{
    layout->setContentsMargins(0, 0, 0, 0);
}

void
TP_VisualContainer::switchWidget(int I)
{
    switch (I)
    {
        case TP_ALBUM_COVER:
        layout->addWidget(tp_AlbumCoverLabel);
        tp_AlbumCoverLabel->setImage();
        break;
    }
}
