#ifndef TP_PLAYLIST_H
#define TP_PLAYLIST_H

#include <QWidget>

namespace Ui { class TP_Playlist; }

class TP_Playlist : public QWidget
{
    Q_OBJECT

public:
    explicit TP_Playlist(QWidget *parent = nullptr);
    ~TP_Playlist();

private slots:
    void on_pushButton_Close_clicked();

private:
    Ui::TP_Playlist *ui;


};

#endif // TP_PLAYLIST_H
