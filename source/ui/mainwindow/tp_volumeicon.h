#ifndef TP_VOLUMEICON_H
#define TP_VOLUMEICON_H

#include <tp_label.h>

class TP_VolumeIcon : public TP_Label
{
    Q_OBJECT

public:
    explicit TP_VolumeIcon(QWidget *parent = nullptr);

    void initialize();
    void setIcon(bool hasSound);

private:
    QPixmap pixmapVolume, pixmapMute;
};

#endif // TP_VOLUMEICON_H
