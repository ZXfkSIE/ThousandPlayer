#ifndef TP_PLAYLISTBOTTOMFRAME_H
#define TP_PLAYLISTBOTTOMFRAME_H

#include <QFrame>

class TP_PlaylistBottomFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TP_PlaylistBottomFrame(QWidget *parent = nullptr);

private:
    static const int TP_BOTTOM_BORDER = 1;
    static const int TP_BORDER_SIZE = 5;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    int isAtBorder(QPoint I_qpt);

    bool b_isBorderBeingPressed;
    bool b_isCursorResize;
    bool b_isExpandingDisabled;
    int cursorPositionType;
    QPoint pressedGlobalPosition;
};

#endif // TP_PLAYLISTBOTTOMFRAME_H
