#include "tp_filelistwidget.h"

#include "tp_globalconst.h"

TP_FileListWidget::TP_FileListWidget(QWidget *parent, const QString &I_qstr) :
    QListWidget { parent }
  , listName { I_qstr }
  , b_isConnected { false }
{
    setMouseTracking(true);

    setDragDropMode(QAbstractItemView::InternalMove);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    QFont currentFont = font();
    currentFont.setPointSize(10);
    setFont(currentFont);

    setStyleSheet("color: rgb(255, 255, 255);");
}

void
TP_FileListWidget::setListName(const QString &I_qstr)
{
    listName = I_qstr;
}

QString
TP_FileListWidget::getListName()
{
    return listName;
}

void
TP_FileListWidget::setConnected()
{
    b_isConnected = true;
}

bool
TP_FileListWidget::isConnected()
{
    return b_isConnected;
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_FileListWidget::dropEvent(QDropEvent *event)
{
    QListWidget::dropEvent(event);
    emit signal_dropped();
}
