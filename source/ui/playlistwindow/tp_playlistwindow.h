#ifndef TP_PLAYLISTWINDOW_H
#define TP_PLAYLISTWINDOW_H

#include "tp_globalenum.h"

#include <QFont>
#include <QListWidget>

class TP_FileListWidget;
class TP_Menu;
class TP_ProgressDialog;
class TP_ReplayGainScanProgress;

class QJsonDocument;
class QThreadPool;

namespace Ui { class TP_PlaylistWindow; }

class TP_PlaylistWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TP_PlaylistWindow( QWidget *parent = nullptr );
    ~TP_PlaylistWindow();

    void initializePlaylist();

    void setCurrentItem( QListWidgetItem *I_item );
    void setCurrentItemBold();
    void unsetCurrentItemBold();
    void refreshItemShowingTitle( QListWidgetItem *I_item );

    QListWidgetItem * getCurrentItem();
    QListWidgetItem * getNextItem();
    QListWidgetItem * getPreviousItem();
    QListWidgetItem * getNextItem_shuffle();
    QListWidgetItem * getPreviousItem_shuffle();

signals:
    void signal_moveWindow( QWidget *window, const QRect &geometry );
    void signal_resizeWindow( QWidget *window, const QRect &geometry, TP::CursorPositionType resizeType );
    void signal_windowChanged();

    void signal_shown();
    void signal_hidden();

    // Generated from FileListWidget
    void signal_currentItemRemoved();
    void signal_itemDoubleClicked( QListWidgetItem *I_item );

public slots:
    void slot_clearPreviousAndNext();

    void slot_activateWindow();

    void slot_refreshFont();

private slots:
    void slot_titleBarMoved( const QRect &I_geometry );
    void slot_resizeWindow( const QRect &I_geometry, TP::CursorPositionType I_resizeType );
    void slot_windowChanged();

    void slot_fileListRemoved( TP_FileListWidget *I_fileListWidget );
    void slot_fileListCreated( TP_FileListWidget *I_fileListWidget );
    void slot_fileListSwitched( TP_FileListWidget *I_fileListWidget );

    void slot_currentItemRemoved();
    void slot_itemDoubleClicked( QListWidgetItem *I_item );

    void slot_refreshShowingTitle( int I_idx_min, int I_idx_max );

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
    void on_action_sortByLastModified_triggered();
    void on_action_sortByAlbum_triggered();
    void on_action_sortByArtist_triggered();
    void on_action_sortByTitle_triggered();
    void on_action_setDescending_triggered( bool I_isChecked );

    void on_action_find_triggered();
    void on_action_findNext_triggered();

private:
    Ui::TP_PlaylistWindow *ui;

    void showEvent( QShowEvent *event ) override;
    void hideEvent( QHideEvent *event ) override;

    void initializeMenu();
    void initializeConnection();

    bool createPlaylistFromJSON( const QJsonDocument &I_jDoc );
    void storePlaylist();

    TP_FileListWidget * currentFileListWidget();
    void                addFilesToCurrentList( const QList< QUrl > &I_urlList );

    TP_Menu *menu_Add;
    TP_Menu *menu_Remove;
    TP_Menu *menu_Select;
    TP_Menu *menu_Sort;
    TP_Menu *menu_Find;

    QThreadPool                 *threadPool;
    TP_ProgressDialog           *progressDialog;
    TP_ReplayGainScanProgress   *replayGainScanProgress;

    bool b_isDescending;

    const QString key_listName      { "listName" };
    const QString key_fileList      { "fileList" };
    const QString key_sourceType    { "sourceType" };
    const QString key_url           { "url" };
};

#endif // TP_PLAYLIST_H
