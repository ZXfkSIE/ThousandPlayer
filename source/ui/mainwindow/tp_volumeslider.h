#ifndef TP_VOLUMESLIDER_H
#define TP_VOLUMESLIDER_H

#include <QSlider>

class TP_VolumeSlider : public QSlider
{
    Q_OBJECT
public:
    explicit TP_VolumeSlider( QWidget *parent = nullptr );

    void setInvertMouseWheel( bool b );

private:
    void mouseReleaseEvent  ( QMouseEvent *event ) override;
    void wheelEvent         ( QWheelEvent *event ) override;

    bool b_invertMouseWheel;
};

#endif // TP_VOLUMESLIDER_H
