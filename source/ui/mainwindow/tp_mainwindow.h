#ifndef TP_MAINWINDOW_H
#define TP_MAINWINDOW_H

#include "tp_globalenum.h"

#include <QWidget>

namespace Ui { class TP_MainWindow; }

class TP_MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TP_MainWindow(QWidget *parent = nullptr);
    ~TP_MainWindow();

signals:
    void signal_openPlaylistWindow();
    void signal_hidePlaylistWindow();

public slots:
    void slot_PlaylistWindow_Shown();
    void slot_PlaylistWindow_Hidden();

private slots:
    void on_pushButton_Exit_clicked() const;
    void on_pushButton_Expand_clicked();
    void on_pushButton_Playlist_clicked();

private:
    Ui::TP_MainWindow *ui;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    void setIcon_Repeat();
    TP::CursorPositionType isAtBorder(QPoint I_point) const;

    bool b_isBorderBeingPressed;
    bool b_isCursorResize;
    bool b_isExpandingDisabled;

    bool b_isPlaylistWindowShown;

    TP::CursorPositionType cursorPositionType;
    QPoint pressedGlobalPosition;
};
#endif // TP_MAINWINDOW_H
