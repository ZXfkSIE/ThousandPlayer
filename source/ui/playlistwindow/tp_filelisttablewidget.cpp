#include "tp_filelisttablewidget.h"

#include <QHeaderView>

TP_FileListTableWidget::TP_FileListTableWidget(QWidget *parent, QString I_qstr) :
    QTableWidget { parent }
  , listName { I_qstr }
{
    setMouseTracking(true);

    verticalHeader()->hide();
    horizontalHeader()->hide();
    setColumnCount(6);                  // 0: title , 1: length, 2: artist, 3: album title, 4: filename, 5: path

    // Hide last 4 columns
    for(int idx = 2; idx <= columnCount(); idx++)
        hideColumn(idx);

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
