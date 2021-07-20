#ifndef TP_FILELISTTABLEVIEW_H
#define TP_FILELISTTABLEVIEW_H

#include <QTableView>

class TP_FileListTableView : public QTableView
{
    Q_OBJECT
public:
    TP_FileListTableView(QWidget *parent = nullptr);
};

#endif // TP_FILELISTTABLEVIEW_H
