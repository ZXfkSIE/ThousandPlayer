#include "tp_filelisttablewidget.h"

#include "tp_globalconst.h"

#include <QHeaderView>
#include <QScrollBar>

TP_FileListTableWidget::TP_FileListTableWidget(QWidget *parent, QString I_qstr) :
    QTableWidget { parent }
  , listName { I_qstr }
{
    setMouseTracking(true);

    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setDragDropMode(QAbstractItemView::InternalMove);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    setShowGrid(false);
    horizontalScrollBar()->hide();
    verticalHeader()->hide();

    horizontalHeader()->hide();
    setColumnCount(8);                  // 0: No., 1: description, 2: duration, 3: artist,
                                        // 4: title, 5: album, 6: filename, 7: path

    // Hide last 5 columns
    for(int idx = 3; idx <= columnCount(); idx++)
        hideColumn(idx);

    horizontalHeader()->setSectionResizeMode(TP::index_Description, QHeaderView::Stretch);

    setStyleSheet("color: rgb(255, 255, 255);");
}

void
TP_FileListTableWidget::setListName(QString I_qstr)
{
    listName = I_qstr;
}

QString
TP_FileListTableWidget::getListName()
{
    return listName;
}
