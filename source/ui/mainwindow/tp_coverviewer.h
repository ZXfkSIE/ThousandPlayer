#ifndef TP_COVERVIEWER_H
#define TP_COVERVIEWER_H

#include <QDialog>

namespace Ui { class TP_CoverViewer; }

class TP_CoverViewer : public QDialog
{
    Q_OBJECT

public:
    explicit TP_CoverViewer( QDialog *parent = nullptr );
    ~TP_CoverViewer();

    void setImage( const QPixmap &I_pixmap );

private slots:
    void on_pushButton_ZoomIn_clicked();
    void on_pushButton_ZoomOut_clicked();
    void on_pushButton_OriginalSize_clicked();
    void on_pushButton_Maximize_clicked();

private:
    Ui::TP_CoverViewer *ui;

    void wheelEvent( QWheelEvent *event ) override;

    void scaleImage( float I_multiplier );

    QPixmap pixmap;
    float scaleFactor;

    QRect   originalGeometry;
    bool    b_isMaximized;
};

#endif // TP_COVERVIEWER_H
