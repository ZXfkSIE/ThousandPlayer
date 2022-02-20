#ifndef TP_LYRICSVIEWER_H
#define TP_LYRICSVIEWER_H

#include <QListWidget>
#include <QUrl>

class TP_Menu;

class TP_LyricsViewer : public QListWidget
{
    Q_OBJECT

public:
    explicit TP_LyricsViewer( QWidget *parent = nullptr );

    void updatePosition( qint64 I_ms );
    void readLyricsFile( const QUrl &I_url );
    void reloadLyricsFile( const QUrl &I_url );
    void refreshFont();

signals:
    void signal_switchToLyricsEditor( const QUrl &url );
    void signal_lyricsDoubleClicked( qint64 ms );

private slots:
    void slot_switchToLyricsEditor();

private:
    void mouseMoveEvent         ( QMouseEvent *event ) override;
    void mouseDoubleClickEvent  ( QMouseEvent *event ) override;

    void contextMenuEvent       ( QContextMenuEvent *event ) override;

    void initializeUI();
    void initializeMenu();

    void sortByTimestamp();
    int findItemByTimestamp( qint64 I_ms );

    void setCurrentItemBold();
    void unsetCurrentItemBold();

    TP_Menu *rightClickMenu;
    QAction *action_switchToLyricsEditor;

    bool b_hasLrcFile;
    qint64 currentPosition;
    int currentIdx;
    QUrl currentLyricsURL;
};

#endif // TP_LYRICSVIEWER_H
