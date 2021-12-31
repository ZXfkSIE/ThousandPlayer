#ifndef TP_LYRICSWINDOW_H
#define TP_LYRICSWINDOW_H

#include "tp_globalenum.h"

#include <QWidget>

class TP_LyricsViewer;

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
    void signal_moveWindow( QWidget *window, const QRect &geometry );
    void signal_resizeWindow( QWidget *window, const QRect &geometry, TP::ResizeType resizeType );
    void signal_windowChanged();

    void signal_shown();
    void signal_hidden();

public slots:
    void slot_changeFont();

private slots:
    void slot_titleBarMoved( const QRect &I_geometry );
    void slot_resizeWindow( const QRect &I_geomtry, TP::ResizeType I_resizeType );
    void slot_windowChanged();

private:
    Ui::TP_LyricsWindow *ui;

    void showEvent( QShowEvent *event ) override;
    void hideEvent( QHideEvent *event ) override;

    void initializeConnection();
    void initializeUI();

    TP_LyricsViewer *lyricsViewer;
};

#endif // TP_LYRICSWINDOW_H
