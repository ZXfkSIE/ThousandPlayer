#ifndef TP_COVERLABEL_H
#define TP_COVERLABEL_H

#include <QLabel>

class TP_CoverViewer;
class TP_Menu;

class TP_CoverLabel : public QLabel
{
    Q_OBJECT

public:
    explicit TP_CoverLabel( QWidget *parent );
    ~TP_CoverLabel();
    void setImage();
    void setImage( const QPixmap &I_pixmap );

private slots:
    void slot_viewCoverImage();

private:
    void mouseDoubleClickEvent( QMouseEvent *event ) override;
    void contextMenuEvent( QContextMenuEvent *event ) override;

    void initializeMenu();

    TP_CoverViewer *coverViewer;

    TP_Menu *menu_rightClick;
    QAction *act_viewCoverImage;
};

#endif // TP_COVERLABEL_H
