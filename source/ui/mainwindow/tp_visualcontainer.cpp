#include "tp_visualcontainer.h"

#include "tp_globalconst.h"
#include "tp_albumcoverlabel.h"

#include <QHBoxLayout>

TP_VisualContainer::TP_VisualContainer(QWidget *parent) :
    QWidget( parent )
  , layout( new QHBoxLayout{this} )
{
    layout->setContentsMargins(0, 0, 0, 0);
}

// Need to be executed manually after the initialization of UI.
void
TP_VisualContainer::initialize()
{
    tp_AlbumCoverLabel = new TP_AlbumCoverLabel{ this, width() };
}

void
TP_VisualContainer::switchWidget(TP::VisualContainerType I_type)
{
    switch (I_type)
    {
    case TP::albumCover:
        layout->addWidget(tp_AlbumCoverLabel);
        tp_AlbumCoverLabel->setImage();
        break;

    default:
        break;
    }
}
