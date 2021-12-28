#ifndef TP_LYRICSSTACKEDWIDGET_H
#define TP_LYRICSSTACKEDWIDGET_H

#include "tp_globalenum.h"

#include <QStackedWidget>

class TP_LyricsStackedWidget : public QStackedWidget
{
    Q_OBJECT

public:
    explicit TP_LyricsStackedWidget( QWidget *parent = nullptr );

signals:
    void signal_resizeWindow( const QRect &newGeometry, TP::ResizeType resizeType );
    void signal_windowChanged();

private:
    void mousePressEvent( QMouseEvent *event ) override;
    void mouseMoveEvent( QMouseEvent *event ) override;
    void mouseReleaseEvent( QMouseEvent *event ) override;

    bool b_isBorderBeingPressed;
    bool b_isCursorResize;

    TP::CursorPositionType cursorPositionType;
};

#endif // TP_LYRICSSTACKEDWIDGET_H
