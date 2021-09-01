#ifndef TP_TIMESLIDER_H
#define TP_TIMESLIDER_H

#include <QSlider>

class TP_TimeSlider : public QSlider
{
    Q_OBJECT
public:
    explicit TP_TimeSlider( QWidget *parent = nullptr );

    void setInvertMouseWheel( bool b );

signals:
    void signal_mouseReleased( int second );

private:
    void mouseReleaseEvent  ( QMouseEvent *event ) override;
    void wheelEvent         ( QWheelEvent *event ) override;

    bool b_invertMouseWheel;
};

#endif // TP_TIMESLIDER_H
