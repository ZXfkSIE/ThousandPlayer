#ifndef TP_FILELISTWIDGET_H
#define TP_FILELISTWIDGET_H

#include "tp_globalenum.h"

#include <QListWidget>

class TP_Menu;

class QProgressDialog;

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

    void sortByData( const int role , const bool isDescending );

    void searchByData( const QString    &I_qstr_keyword,
                       const qsizetype  startingIndex,
                       const bool       isFilenameSearched,
                       const bool       isAlbumSearched,
                       const bool       isArtistSearched,
                       const bool       isTitleSearched);
    void findNext();

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

    void quickSort( const int role, const int left, const int right, const bool isDescending );

    TP_Menu *menu_rightClick;
    QAction *action_remove;

    // previousItem & currentItem are only used in shuffle mode
    QListWidgetItem *previousItem, *nextItem;

    QString qstr_listName;
    bool    b_isConnected;
    bool    b_isLeftButtonPressed;

    QProgressDialog *progressDialog;

    QString         qstr_keyword;
    bool            b_isFilenameSearched;
    bool            b_isAlbumSearched;
    bool            b_isArtistSearched;
    bool            b_isTitleSearched;
};

#endif // TP_FILELISTWIDGET_H
