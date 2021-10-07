#ifndef TP_VOLUMEICON_H
#define TP_VOLUMEICON_H

#include <QLabel>

class TP_VolumeIcon : public QLabel
{
    Q_OBJECT

public:
    explicit TP_VolumeIcon( QWidget *parent = nullptr );

    void initialize();
    void setIcon( int I_volume );

signals:
    void signal_setVolume( int I_volume );

private:
    void mousePressEvent( QMouseEvent *event ) override;

    QPixmap pixmap_Volume, pixmap_Mute;

    int volume, originalVolume;
};

#endif // TP_VOLUMEICON_H
