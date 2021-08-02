#ifndef TP_PLAYLISTWINDOW_H
#define TP_PLAYLISTWINDOW_H

#include <QListWidget>

#include <vector>

class TP_FileListWidget;

class QHBoxLayout;

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

    void initializePlaylist();
    void storePlaylist();

    QHBoxLayout *layout_FileListFrame;

    TP_FileListWidget *currentListWidget;
    std::vector<TP_FileListWidget *> vector_FileListWidget;
};

#endif // TP_PLAYLIST_H
