#ifndef TP_PLAYLISTBOTTOMFRAME_H
#define TP_PLAYLISTBOTTOMFRAME_H

#include "tp_globalenum.h"

#include <QFrame>

class TP_PlaylistBottomFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TP_PlaylistBottomFrame( QWidget *parent = nullptr );

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

#endif // TP_PLAYLISTBOTTOMFRAME_H
