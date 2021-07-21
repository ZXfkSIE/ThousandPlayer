#ifndef TP_PLAYLISTCONTAINER_H
#define TP_PLAYLISTCONTAINER_H

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

    int isAtBorder(QPoint I_point) const;

    bool b_isBorderBeingPressed;
    bool b_isCursorResize;
    bool b_isExpandingDisabled;
    int cursorPositionType;
    QPoint pressedGlobalPosition;
};

#endif // TP_PLAYLISTCONTAINER_H
