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
    void signal_resizeWindow( QRect newGeometry, TP::ResizeType resizeType );
    void signal_leftButtonReleased();

private:
    void mousePressEvent( QMouseEvent *event ) override;
    void mouseMoveEvent( QMouseEvent *event ) override;
    void mouseReleaseEvent( QMouseEvent *event ) override;

    TP::CursorPositionType isAtBorder(const QPoint &I_point) const;

    bool                    b_isBorderBeingPressed;
    bool                    b_isCursorResize;
    bool                    b_isExpandingDisabled;
    TP::CursorPositionType  cursorPositionType;
};

#endif // TP_PLAYLISTBOTTOMFRAME_H
