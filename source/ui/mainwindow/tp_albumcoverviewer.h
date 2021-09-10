#ifndef TP_ALBUMCOVERVIEWER_H
#define TP_ALBUMCOVERVIEWER_H

#include <QWidget>

class TP_AlbumCoverViewer : public QWidget
{
    Q_OBJECT

public:
    explicit TP_AlbumCoverViewer(QWidget *parent = nullptr);
    ~TP_AlbumCoverViewer();
};

#endif // TP_ALBUMCOVERVIEWER_H
