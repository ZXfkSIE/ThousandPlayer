#include "tp_visualcontainer.h"

#include "tp_globalconst.h"
#include "tp_albumcoverlabel.h"

#include <QHBoxLayout>

TP_VisualContainer::TP_VisualContainer(QWidget *parent) :
    QWidget( parent )
  , layout { new QHBoxLayout{ this } }
{
    layout->setContentsMargins(0, 0, 0, 0);
}

// Need to be executed manually after the initialization of UI
// to get the correct return value of width().
void
TP_VisualContainer::initialize()
{
    albumCoverLabel = new TP_AlbumCoverLabel{ this, width() };
}

void
TP_VisualContainer::switchWidget(TP::VisualContainerType type)
{
    switch ( type )
    {
    case TP::albumCover:
        layout->addWidget( albumCoverLabel );
        albumCoverLabel->setImage();
        break;

    default:
        break;
    }
}
