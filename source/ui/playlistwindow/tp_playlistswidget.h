#ifndef TP_PLAYLISTSWIDGET_H
#define TP_PLAYLISTSWIDGET_H

#include <QListWidget>

class TP_PlaylistsWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit TP_PlaylistsWidget(QWidget *parent = nullptr);
};

#endif // TP_PLAYLISTSWIDGET_H
