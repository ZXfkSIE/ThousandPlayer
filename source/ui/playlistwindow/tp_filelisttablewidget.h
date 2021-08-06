#ifndef TP_FILELISTTABLEWIDGET_H
#define TP_FILELISTTABLEWIDGET_H

#include <QTableWidget>

class TP_FileListTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit TP_FileListTableWidget(QWidget *parent, QString I_qstr);

    void setListName(QString I_qstr);
    QString getListName();

private:
    //void dropEvent(QDropEvent *event) override;

    QString listName;
};

#endif // TP_FILELISTTABLEWIDGET_H
