#include "tp_volumeicon.h"

#include <QIcon>

TP_VolumeIcon::TP_VolumeIcon(QWidget *parent) :
    TP_Label(parent)
{
    pixmapVolume = QIcon(":/image/icon_Volume.svg")
                   .pixmap(QSize(22, 22));
    pixmapMute = QIcon(":/image/icon_Mute.svg")
                 .pixmap(QSize(22, 22));
}

void
TP_VolumeIcon::setIcon(int I)
{
    if(I)
        setPixmap(pixmapVolume);
    else
        setPixmap(pixmapMute);
}
