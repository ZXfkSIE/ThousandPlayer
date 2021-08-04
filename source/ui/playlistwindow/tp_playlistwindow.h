#ifndef TP_PLAYLISTWINDOW_H
#define TP_PLAYLISTWINDOW_H

#include <QListWidget>

#include <vector>

class TP_FileListTableWidget;
class TP_Menu;

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

    void on_action_AddFile_triggered();

private:
    Ui::TP_PlaylistWindow *ui;

    void initializePlaylist();
    void initializeMenu();

    void storePlaylist();

    QHBoxLayout *layout_FileListFrame;

    TP_FileListTableWidget *currentFileListTableWidget;
    std::vector<TP_FileListTableWidget *> vector_FileListTableWidget;

    TP_Menu *menu_Add;
};

#endif // TP_PLAYLIST_H
