#ifndef TP_ALBUMCOVERLABEL_H
#define TP_ALBUMCOVERLABEL_H

#include <tp_label.h>

class TP_AlbumCoverLabel : public TP_Label
{
    Q_OBJECT

public:
    explicit TP_AlbumCoverLabel(QWidget *parent, int I_size);
    void setImage();
    void setImage(QPixmap I_pixmap);

    QSize sizeHint() const override;

private:
    int size;
};

#endif // TP_ALBUMCOVERLABEL_H
