#ifndef TP_RUNNABLE_FILEREADER_H
#define TP_RUNNABLE_FILEREADER_H

#include <QObject>
#include <QRunnable>

class QListWidgetItem;

class TP_Runnable_FileReader : public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit TP_Runnable_FileReader( QListWidgetItem *I_item );

    void run() override;

signals:
    void signal_onFinish();

private:
    QListWidgetItem *item;
};

#endif // TP_RUNNABLE_FILEREADER_H
