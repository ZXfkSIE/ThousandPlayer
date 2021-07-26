#ifndef TP_PLAYLISTCONTAINER_H
#define TP_PLAYLISTCONTAINER_H

#include "tp_globalenum.h"

#include <QWidget>

class TP_PlaylistContainer : public QWidget
{
    Q_OBJECT

public:
    explicit TP_PlaylistContainer(QWidget *parent = nullptr);

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    TP::CursorPositionType isAtBorder(QPoint I_point) const;

    bool b_isBorderBeingPressed;
    bool b_isCursorResize;
    bool b_isExpandingDisabled;
    TP::CursorPositionType cursorPositionType;
    QPoint pressedGlobalPosition;
};

#endif // TP_PLAYLISTCONTAINER_H
