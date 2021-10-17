#ifndef TP_RUNNABLE_FILEREADER_H
#define TP_RUNNABLE_FILEREADER_H

#include <QRunnable>

class QListWidgetItem;

class TP_Runnable_FileReader : public QRunnable
{

public:
    explicit TP_Runnable_FileReader( QListWidgetItem *I_item );

    void run() override;

private:
    QListWidgetItem *item;
};

#endif // TP_RUNNABLE_FILEREADER_H
