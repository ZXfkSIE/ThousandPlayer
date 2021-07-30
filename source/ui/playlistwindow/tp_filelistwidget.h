#ifndef TP_FILELISTWIDGET_H
#define TP_FILELISTWIDGET_H

#include <QListWidget>

class QStandardItemModel;

class TP_FileListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit TP_FileListWidget(QWidget *parent = nullptr);
};

#endif // TP_FILELISTWIDGET_H
