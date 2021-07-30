#ifndef TP_PLAYLISTWINDOW_H
#define TP_PLAYLISTWINDOW_H

#include <QWidget>

#include <vector>

class QStandardItemModel;

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

    QStandardItemModel* model_ListOfPlaylists;

    std::vector<
        std::pair<std::string, QStandardItemModel*>
    > vector_model_Playlist;
};

#endif // TP_PLAYLIST_H
