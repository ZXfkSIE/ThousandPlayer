#ifndef TP_ALBUMCOVERLABEL_H
#define TP_ALBUMCOVERLABEL_H

#include <QLabel>

class TP_Menu;

class TP_AlbumCoverLabel : public QLabel
{
    Q_OBJECT

public:
    explicit TP_AlbumCoverLabel( QWidget *parent );
    void setImage();
    void setImage( const QPixmap &I_pixmap );

private slots:
    void slot_viewCoverImage();

private:
    void mouseDoubleClickEvent( QMouseEvent *event ) override;
    void contextMenuEvent( QContextMenuEvent *event ) override;

    void initializeMenu();

    TP_Menu *menu_rightClick;
    QAction *act_viewCoverImage;

    QPixmap currentPixmap;
};

#endif // TP_ALBUMCOVERLABEL_H
