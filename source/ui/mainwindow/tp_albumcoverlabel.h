#ifndef TP_ALBUMCOVERLABEL_H
#define TP_ALBUMCOVERLABEL_H

#include <QLabel>

class TP_AlbumCoverLabel : public QLabel
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
