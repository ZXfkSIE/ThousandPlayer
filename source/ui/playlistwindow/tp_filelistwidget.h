#ifndef TP_FILELISTWIDGET_H
#define TP_FILELISTWIDGET_H

#include <QListWidget>

class QStandardItemModel;

class TP_FileListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit TP_FileListWidget(QWidget *parent,
                               QString I_qstr_Name);

private:
    QString qstr_Name;
};

#endif // TP_FILELISTWIDGET_H
