#ifndef TP_CONFIG_H
#define TP_CONFIG_H

#include <QObject>

class TP_Config : public QObject
{
    Q_OBJECT
public:
    explicit TP_Config( QObject *parent = nullptr );

signals:

};

#endif // TP_CONFIG_H
