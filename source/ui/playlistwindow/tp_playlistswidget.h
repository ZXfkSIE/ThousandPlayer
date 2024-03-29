﻿#ifndef TP_PLAYLISTSWIDGET_H
#define TP_PLAYLISTSWIDGET_H

#include <QListWidget>

class TP_FileListWidget;
class TP_Menu;

class TP_PlaylistsWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit TP_PlaylistsWidget( QWidget *parent = nullptr );

    QListWidgetItem * addNewList( const QString &I_listName );

    void refreshFont();

signals:
    void signal_fileListRemoved( TP_FileListWidget *I_fileListWidget );
    void signal_fileListCreated( TP_FileListWidget *I_fileListWidget );
    void signal_fileListSwitched( TP_FileListWidget *I_fileListWidget );

private slots:
    void slot_removeCurrentItem();
    void slot_renameCurrentItem();
    void slot_addItemFromContextMenu();

private:
    void contextMenuEvent       ( QContextMenuEvent *event ) override;
    void mouseDoubleClickEvent  ( QMouseEvent *event ) override;

    void initializeMenu();

    void switchVisibleFileList( QListWidgetItem *I_item );
    void setItemBold( QListWidgetItem *I_item );
    void unsetItemBold( QListWidgetItem *I_item );

    TP_Menu *menu_rightClick;
    QAction *action_remove;
    QAction *action_rename;
    QAction *action_add;

    QListWidgetItem *currentVisibleItem;
};

#endif // TP_PLAYLISTSWIDGET_H
