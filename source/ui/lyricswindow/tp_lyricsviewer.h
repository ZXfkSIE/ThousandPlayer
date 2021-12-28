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

private:
    void sortByTimestamp();

    bool b_hasLrcFile;

    QListWidgetItem *currentItem;
};

#endif // TP_LYRICSVIEWER_H
