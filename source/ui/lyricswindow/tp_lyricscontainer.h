#ifndef TP_LYRICSCONTAINER_H
#define TP_LYRICSCONTAINER_H

#include <QWidget>

class TP_LyricsContainer : public QWidget
{
    Q_OBJECT

public:
    explicit TP_LyricsContainer( QWidget *parent = nullptr );

private:
    void paintEvent( QPaintEvent * ) override;

};

#endif // TP_LYRICSCONTAINER_H
