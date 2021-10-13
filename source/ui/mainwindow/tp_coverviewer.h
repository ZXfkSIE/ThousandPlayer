#ifndef TP_COVERVIEWER_H
#define TP_COVERVIEWER_H

#include <QWidget>

namespace Ui { class TP_CoverViewer; }

class TP_CoverViewer : public QWidget
{
    Q_OBJECT

public:
    explicit TP_CoverViewer( QWidget *parent = nullptr );
    ~TP_CoverViewer();

    void setImage( const QPixmap &I_pixmap );

private slots:
    void on_pushButton_ZoomIn_clicked();
    void on_pushButton_ZoomOut_clicked();
    void on_pushButton_OriginalSize_clicked();

private:
    Ui::TP_CoverViewer *ui;

    void scaleImage( float I_multiplier );

    QPixmap pixmap;
    float scaleFactor;
};

#endif // TP_COVERVIEWER_H
