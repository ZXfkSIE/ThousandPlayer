#include "tp_playlistswidget.h"

#include <QMouseEvent>

TP_PlaylistsWidget::TP_PlaylistsWidget(QWidget *parent) :
    QListWidget(parent)
{
    setMouseTracking(true);
    setDragDropMode(QAbstractItemView::InternalMove);
}
