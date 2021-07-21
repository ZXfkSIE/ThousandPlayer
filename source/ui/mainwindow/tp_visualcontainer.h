#ifndef TP_VISUALCONTAINER_H
#define TP_VISUALCONTAINER_H

#include <tp_container.h>

class TP_AlbumCoverLabel;

class QHBoxLayout;

class TP_VisualContainer : public TP_Container
{
    Q_OBJECT

public:
    explicit TP_VisualContainer(QWidget *parent = nullptr);
    void switchWidget(int I);

private:
    QHBoxLayout *layout;
    TP_AlbumCoverLabel *tp_AlbumCoverLabel;
};

#endif // TP_VISUALCONTAINER_H
