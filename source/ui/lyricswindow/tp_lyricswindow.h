#ifndef TP_LYRICSWINDOW_H
#define TP_LYRICSWINDOW_H

#include "tp_globalenum.h"

#include <QWidget>

class TP_LyricsEditor;
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

    void readLyricsFileFromCurrentItem();
    void clearLyricsViewer();
    void updatePosition( qint64 I_ms );

    bool saveLyricsFileBeforeQuit();

signals:
    void signal_moveWindow( QWidget *window, const QRect &geometry );
    void signal_resizeWindow( QWidget *window, const QRect &geometry, TP::CursorPositionType resizeType );
    void signal_windowChanged();
    void signal_lyricsDoubleClicked( qint64 ms );

    void signal_shown();
    void signal_hidden();

public slots:
    void slot_activateWindow();

    void slot_refreshFont();

private slots:
    void slot_titleBarMoved( const QRect &I_geometry );
    void slot_resizeWindow( const QRect &I_geomtry, TP::CursorPositionType I_resizeType );
    void slot_windowChanged();

    void slot_lyricsDoubleClicked( qint64 I_ms );
    void slot_switchToLyricsViewer( const QUrl &I_url );
    void slot_switchToLyricsEditor( const QUrl &I_url );

    void on_pushButton_Close_clicked();

private:
    Ui::TP_LyricsWindow *ui;

    void showEvent( QShowEvent *event ) override;
    void hideEvent( QHideEvent *event ) override;

    void initializeConnection();
    void initializeUI();

    TP_LyricsViewer *lyricsViewer;
    TP_LyricsEditor *lyricsEditor;
};

#endif // TP_LYRICSWINDOW_H
