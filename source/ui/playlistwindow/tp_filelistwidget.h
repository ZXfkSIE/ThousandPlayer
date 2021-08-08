#ifndef TP_FILELISTWIDGET_H
#define TP_FILELISTWIDGET_H

#include <QListWidget>

class TP_FileListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit TP_FileListWidget(QWidget *parent, QString I_qstr);

    void setListName(QString I_qstr);
    QString getListName();

signals:
    void signal_dropped();

private:
    void dropEvent(QDropEvent *event) override;

    QString listName;
};

#endif // TP_FILELISTWIDGET_H
