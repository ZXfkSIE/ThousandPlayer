#ifndef TP_LYRICSVIEWER_H
#define TP_LYRICSVIEWER_H

#include <QListWidget>

class TP_LyricsViewer : public QListWidget
{
    Q_OBJECT

public:
    explicit TP_LyricsViewer( QWidget *parent = nullptr );

    void updatePosition( qint64 I_ms );
    void readLrcFile( const QString &I_qstr_Path );
    void changeFont( const QFont &I_font );

signals:
    void signal_lyricsDoubleClicked( qint64 ms );

private:
    void mouseMoveEvent         ( QMouseEvent *event ) override;
    void mouseDoubleClickEvent  ( QMouseEvent *event ) override;

    void initializeUI();

    void sortByTimestamp();
    int findItemByTimestamp( qint64 I_ms );

    void setCurrentItemBold();
    void unsetCurrentItemBold();

    bool b_hasLrcFile;
    int currentIdx;
};

#endif // TP_LYRICSVIEWER_H
