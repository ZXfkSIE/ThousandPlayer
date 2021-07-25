#include "tp_volumeicon.h"

#include <QIcon>

TP_VolumeIcon::TP_VolumeIcon(QWidget *parent) :
    TP_Label(parent)
{

}

// Need to be executed manually after the initialization of UI.
void
TP_VolumeIcon::initialize()
{
    pixmapVolume = QIcon(":/image/icon_Volume.svg")
                   .pixmap(QSize(width(), height()));
    pixmapMute = QIcon(":/image/icon_Mute.svg")
                 .pixmap(QSize(width(), height()));
}


void
TP_VolumeIcon::setIcon(int I)
{
    if(I)                           // TP_VOLUME = 1
        setPixmap(pixmapVolume);
    else                            // TP_MUTE = 0
        setPixmap(pixmapMute);
}
