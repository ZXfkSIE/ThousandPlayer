#include "tp_globalfunction.h"

#include <QFileInfo>

QString
TP::extension( const QString &path )
{
    return QFileInfo { path }.suffix().toLower();
}
