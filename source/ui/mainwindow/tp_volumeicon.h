#ifndef TP_VOLUMEICON_H
#define TP_VOLUMEICON_H

#include <QLabel>

class TP_VolumeIcon : public QLabel
{
    Q_OBJECT

public:
    explicit TP_VolumeIcon(QWidget *parent = nullptr);

    void initialize();
    void setIcon(int volume);

private:
    QPixmap pixmap_Volume, pixmap_Mute;
};

#endif // TP_VOLUMEICON_H
