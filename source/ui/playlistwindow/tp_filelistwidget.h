#ifndef TP_FILELISTWIDGET_H
#define TP_FILELISTWIDGET_H

#include "tp_globalenum.h"

#include <QListWidget>

class TP_Menu;

class TP_FileListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit TP_FileListWidget( QWidget *parent, const QString &I_qstr );

    void setListName( const QString &I_qstr );
    QString getListName() const;

    void setCurrentItemBold();
    void unsetCurrentItemBold();
    QListWidgetItem * getCurrentItem();
    QListWidgetItem * getNextItem();
    QListWidgetItem * getPreviousItem();
    QListWidgetItem * getNextItem_shuffle();
    QListWidgetItem * getPreviousItem_shuffle();

    void clearPreviousAndNext();
    void refreshShowingTitle( int idx_Min, const int idx_Max );

    void clearUnselectedItems();
    void clearInaccessibleItems();
    void clearAllItems();
    void deleteSelectedItems();

    void reverseSelection();

    void sortByData( const int role );

signals:
    void signal_currentItemRemoved();

public slots:
    void slot_clearSelectedItems();

private:
    void dropEvent( QDropEvent *event ) override;
    void mousePressEvent( QMouseEvent *event ) override;
    void mouseDoubleClickEvent( QMouseEvent *event ) override;
    void mouseMoveEvent( QMouseEvent *event ) override;
    void mouseReleaseEvent( QMouseEvent *event ) override;
    void contextMenuEvent( QContextMenuEvent *event ) override;

    void initializeMenu();

    void quickSort( const int role, const int left, const int right );

    TP_Menu *menu_rightClick;
    QAction *action_remove;

    // previousItem & currentItem are only used in shuffle mode
    QListWidgetItem *previousItem, *nextItem;

    QString listName;
    bool    b_isConnected;
    bool    b_isLeftButtonPressed;
};

#endif // TP_FILELISTWIDGET_H
