#include "tp_volumeicon.h"

#include <QIcon>

TP_VolumeIcon::TP_VolumeIcon(QWidget *parent) :
    QLabel(parent)
{

}

// Must be executed manually after the initialization of UI
// to get the correct width and height.
void
TP_VolumeIcon::initialize()
{
    pixmap_Volume = QIcon(":/image/icon_Volume.svg")
            .pixmap(QSize(width(), height()));
    pixmap_Mute = QIcon(":/image/icon_Mute.svg")
            .pixmap(QSize(width(), height()));
}

void
TP_VolumeIcon::setIcon(int I_currentVolume)
{
    if(I_currentVolume)
        setPixmap(pixmap_Volume);
    else
        setPixmap(pixmap_Mute);
}
