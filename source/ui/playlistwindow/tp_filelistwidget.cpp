#include "tp_filelistwidget.h"

TP_FileListWidget::TP_FileListWidget(QWidget *parent, QString I_qstr_Name) :
    QListWidget(parent),
    qstr_Name {I_qstr_Name}
{
    setStyleSheet("color: rgb(255, 255, 255);");
    setSelectionMode(QAbstractItemView::MultiSelection);
}
