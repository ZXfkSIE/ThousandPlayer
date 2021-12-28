#ifndef TP_LYRICSWINDOW_H
#define TP_LYRICSWINDOW_H

#include "tp_globalenum.h"

#include <QWidget>

namespace Ui {
class TP_LyricsWindow;
}

class TP_LyricsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TP_LyricsWindow( QWidget *parent = nullptr );
    ~TP_LyricsWindow();

signals:
    void signal_moveWindow( QWidget *window, const QRect &newGeometry );
    void signal_resizeWindow( QWidget *window, const QRect &newGeometry, TP::ResizeType resizeType );
    void signal_windowChanged();

private slots:
    void slot_titleBarMoved( const QRect &newGeometry );
    void slot_resizeWindow( const QRect &newGeomtry, TP::ResizeType resizeType );
    void slot_windowChanged();

    void slot_changeFontOfWidgets();

private:
    Ui::TP_LyricsWindow *ui;

    void initializeConnection();
};

#endif // TP_LYRICSWINDOW_H
