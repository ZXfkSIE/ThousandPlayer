#ifndef TP_PLAYLISTWINDOW_H
#define TP_PLAYLISTWINDOW_H

#include "tp_globalenum.h"

#include <QFont>
#include <QListWidget>

class TP_FileListWidget;
class TP_Menu;
class TP_ProgressDialog;

class QJsonDocument;

namespace Ui { class TP_PlaylistWindow; }

class TP_PlaylistWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TP_PlaylistWindow( QWidget *parent = nullptr );
    ~TP_PlaylistWindow();

    void setCurrentItem( QListWidgetItem *I_item );
    void setCurrentItemBold();
    void unsetCurrentItemBold();
    void refreshShowingTitle( QListWidgetItem *I_item );

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

    // Generated from FileListWidget
    void signal_currentItemRemoved();
    void signal_itemDoubleClicked( QListWidgetItem *I_item );

public slots:
    void slot_clearPreviousAndNext();

    void slot_activateWindow();

    void slot_changeFontOfLists();

private slots:
    void slot_moveTitleBar( const QRect &newGeometry );
    void slot_leftButtonReleased();

    void slot_resizeWindow( const QRect &newGeomtry, TP::ResizeType resizeType );

    void slot_fileListRemoved( TP_FileListWidget *I_fileListWidget );
    void slot_fileListCreated( TP_FileListWidget *I_fileListWidget );
    void slot_fileListSwitched( TP_FileListWidget *I_fileListWidget );

    void slot_currentItemRemoved();
    void slot_itemDoubleClicked( QListWidgetItem *I_item );

    void on_pushButton_Close_clicked();

    void on_action_addFiles_triggered();
    void on_action_addFolder_triggered();

    void on_action_clearSelectedItems_triggered();
    void on_action_clearUnselectedItems_triggered();
    void on_action_clearAllItems_triggered();
    void on_action_clearInaccessibleItems_triggered();
    void on_action_deleteFromDisk_triggered();

    void on_action_selectAll_triggered();
    void on_action_unselectAll_triggered();
    void on_action_reverseSelection_triggered();

    void on_action_sortByDuration_triggered();
    void on_action_sortByPath_triggered();
    void on_action_sortByFilename_triggered();
    void on_action_sortByDescription_triggered();
    void on_action_sortByAlbum_triggered();
    void on_action_sortByArtist_triggered();
    void on_action_sortByTitle_triggered();
    void on_action_setDescending_triggered( bool checked );

    void on_action_find_triggered();
    void on_action_findNext_triggered();

private:
    Ui::TP_PlaylistWindow *ui;

    void showEvent( QShowEvent *event ) override;
    void hideEvent( QHideEvent *event ) override;

    void initializeMenu();
    void initializeConnection();
    void initializePlaylist();

    void createPlaylistFromJSON( const QJsonDocument &I_jDoc );
    void storePlaylist();

    TP_FileListWidget * currentFileListWidget();
    void                addFilesToCurrentList( const QList< QUrl > &urlList );

    TP_Menu *menu_Add;
    TP_Menu *menu_Remove;
    TP_Menu *menu_Select;
    TP_Menu *menu_Sort;
    TP_Menu *menu_Find;

    TP_ProgressDialog *progressDialog;

    bool b_isDescending;
};

#endif // TP_PLAYLIST_H
