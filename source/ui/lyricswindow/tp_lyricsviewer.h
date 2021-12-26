#ifndef TP_LYRICSVIEWER_H
#define TP_LYRICSVIEWER_H

#include <QListWidget>

class TP_LyricsViewer : public QListWidget
{
    Q_OBJECT

public:
    explicit TP_LyricsViewer( QWidget *parent = nullptr );

    void updatePosition( qint64 ms );
    void readLrcFile( const QString &I_qstr_Path );

private:
    bool b_hasLrcFile;
};

#endif // TP_LYRICSVIEWER_H
