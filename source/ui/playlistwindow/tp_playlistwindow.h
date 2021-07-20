#ifndef TP_PLAYLISTWINDOW_H
#define TP_PLAYLISTWINDOW_H

#include <QWidget>

namespace Ui { class TP_PlaylistWindow; }

class TP_PlaylistWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TP_PlaylistWindow(QWidget *parent = nullptr);
    ~TP_PlaylistWindow();

private slots:
    void on_pushButton_Close_clicked();

private:
    Ui::TP_PlaylistWindow *ui;

};

#endif // TP_PLAYLIST_H
