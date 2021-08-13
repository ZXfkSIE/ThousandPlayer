#ifndef TP_FILELISTWIDGET_H
#define TP_FILELISTWIDGET_H

#include <QListWidget>

class TP_FileListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit TP_FileListWidget(QWidget *parent, const QString &I_qstr);

    void setListName(const QString &I_qstr);
    QString getListName();
    void setConnected();
    bool isConnected();

signals:
    void signal_dropped();

private:
    void dropEvent(QDropEvent *event) override;

    QString listName;
    bool    b_isConnected;
};

#endif // TP_FILELISTWIDGET_H
