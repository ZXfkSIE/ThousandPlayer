#ifndef TP_PLAYLISTWINDOW_H
#define TP_PLAYLISTWINDOW_H

#include "tp_globalenum.h"

#include <QListWidget>

#include <vector>

class TP_FileListWidget;
class TP_Menu;

class QHBoxLayout;
class QMediaPlayer;

namespace Ui { class TP_PlaylistWindow; }

class TP_PlaylistWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TP_PlaylistWindow(QWidget *parent = nullptr);
    ~TP_PlaylistWindow();

    void initializePlaylist();

    void setCurrentItem( QListWidgetItem * I_item );
    void setCurrentItemBold();
    void unsetCurrentItemBold();
    QListWidgetItem * getCurrentItem();
    QListWidgetItem * getNextItem();
    QListWidgetItem * getPreviousItem();
    QListWidgetItem * getNextItem_shuffle();
    QListWidgetItem * getPreviousItem_shuffle();

signals:
    void signal_moveWindow( QWidget *window, QRect newGeometry );
    void signal_leftButtonReleased();

    void signal_resizeWindow( QWidget *window, QRect newGeometry, TP::ResizeType resizeType );

    void signal_shown();
    void signal_hidden();
    void signal_newFilelistWidgetCreated( TP_FileListWidget *I_fileListWidget );
    void signal_refreshShowingTitle( int idx_Min, int idx_Max );

    void signal_currentItemRemoved();

public slots:
    void slot_modeIsNotShuffle();

private slots:
    void slot_moveTitleBar( QRect newGeometry );
    void slot_leftButtonReleased();

    void slot_resizeWindow( QRect newGeomtry, TP::ResizeType resizeType );

    void slot_currentItemRemoved();

    void on_pushButton_Close_clicked();

    void on_action_addFiles_triggered();

    void on_action_clearSelectedItems_triggered();
    void on_action_clearUnselectedItems_triggered();
    void on_action_clearAllItems_triggered();
    void on_action_clearInaccessibleItems_triggered();
    void on_action_deleteFromDisk_triggered();

private:
    Ui::TP_PlaylistWindow *ui;

    void showEvent( QShowEvent *event ) override;
    void hideEvent( QHideEvent *event ) override;

    void initializeMenu();
    void initializeConnection();

    void storePlaylist();

    QHBoxLayout *layout_FileListFrame;

    TP_FileListWidget *currentFileListWidget;
    std::vector<TP_FileListWidget *> vector_FileListWidget;

    TP_Menu *menu_Add;
    TP_Menu *menu_Remove;
};

#endif // TP_PLAYLIST_H
