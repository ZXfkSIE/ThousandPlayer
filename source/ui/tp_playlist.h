﻿#ifndef TP_PLAYLIST_H
#define TP_PLAYLIST_H

#include <QWidget>

namespace Ui {
class TP_Playlist;
}

class TP_Playlist : public QWidget
{
    Q_OBJECT

public:
    explicit TP_Playlist(QWidget *parent = nullptr);
    ~TP_Playlist();

private slots:
    void on_pushButton_Close_clicked();

private:
    Ui::TP_Playlist *ui;

    static const int TP_LEFT_BORDER = -1, TP_RIGHT_BORDER = 1;
    static const unsigned TP_BORDER_SIZE = 5;

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

#endif // TP_PLAYLIST_H
