#ifndef TP_VISUALCONTAINER_H
#define TP_VISUALCONTAINER_H

#include "tp_globalenum.h"

#include <QWidget>

class TP_AlbumCoverLabel;
class QHBoxLayout;

class TP_VisualContainer : public QWidget
{
    Q_OBJECT

public:
    explicit TP_VisualContainer( QWidget *parent = nullptr );

    void initialize();
    void switchWidget( TP::VisualContainerType type );

private:
    QHBoxLayout         *layout;
    TP_AlbumCoverLabel  *albumCoverLabel;
};

#endif // TP_VISUALCONTAINER_H
