#ifndef TP_RUNNABLE_REPLAYGAINSCANNER_H
#define TP_RUNNABLE_REPLAYGAINSCANNER_H

#include <QObject>
#include <QRunnable>

class TP_Runnable_ReplayGainScanner : public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit TP_Runnable_ReplayGainScanner( const QString   &I_qstr_path,
                                            int        I_index );

    void run() override;

signals:
    void signal_onStart( int I_index );
    void signal_onFinish( int I_index, bool I_isSuccessful );

private:
    QString qstr_path;
    int index;
};

#endif // TP_RUNNABLE_REPLAYGAINSCANNER_H
