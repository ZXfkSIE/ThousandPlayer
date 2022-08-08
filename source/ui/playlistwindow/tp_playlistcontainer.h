#ifndef TP_PLAYLISTCONTAINER_H
#define TP_PLAYLISTCONTAINER_H

#include "tp_globalenum.h"

#include <QWidget>

class TP_PlaylistContainer : public QWidget
{
    Q_OBJECT

public:
    explicit TP_PlaylistContainer( QWidget *parent = nullptr );

signals:
    void signal_resizeWindow( const QRect &newGeometry, TP::CursorPositionType resizeType );
    void signal_windowChanged();

private:
    void mousePressEvent( QMouseEvent *event ) override;
    void mouseMoveEvent( QMouseEvent *event ) override;
    void mouseReleaseEvent( QMouseEvent *event ) override;

    bool b_isBorderBeingPressed;
    bool b_isCursorResize;

    TP::CursorPositionType cursorPositionType;
};

#endif // TP_PLAYLISTCONTAINER_H
