#include "tp_filelistwidget.h"

#include "tp_globalconst.h"

TP_FileListWidget::TP_FileListWidget(QWidget *parent, QString I_qstr) :
    QListWidget { parent }
  , listName { I_qstr }
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
TP_FileListWidget::setListName(QString I_qstr)
{
    listName = I_qstr;
}

QString
TP_FileListWidget::getListName()
{
    return listName;
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
