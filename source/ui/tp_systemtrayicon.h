#ifndef TP_SYSTEMTRAYICON_H
#define TP_SYSTEMTRAYICON_H

#include <QSystemTrayIcon>

class TP_SystemTrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    explicit TP_SystemTrayIcon(QObject *parent = nullptr);
};

#endif // TP_SYSTEMTRAYICON_H
