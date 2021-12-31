#ifndef TP_FILELISTWIDGET_H
#define TP_FILELISTWIDGET_H

#include "tp_globalenum.h"

#include <QListWidget>

class TP_Menu;
class TP_ProgressDialog;

class TP_FileListWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit TP_FileListWidget( QWidget *parent );

    void setCurrentItemBold();
    void unsetCurrentItemBold();
    QListWidgetItem * getCurrentItem();
    QListWidgetItem * getNextItem();
    QListWidgetItem * getPreviousItem();
    QListWidgetItem * getNextItem_shuffle();
    QListWidgetItem * getPreviousItem_shuffle();

    void clearPreviousAndNext();
    void refreshShowingTitle( int I_minIdx, int I_maxIdx );

    void clearUnselectedItems();
    void clearInaccessibleItems();
    void clearAllItems();
    void deleteSelectedItems();

    void reverseSelection();

    void sortByData( int I_role, bool I_isDescending );

    void searchByData( const QString &  I_qstr_keyword,
                       qsizetype        I_startingIdx,
                       bool             I_isFilenameSearched,
                       bool             I_isAlbumSearched,
                       bool             I_isArtistSearched,
                       bool             I_isTitleSearched );
    void findNext();

signals:
    void signal_currentItemRemoved();

public slots:
    void slot_clearSelectedItems();

private:
    void dropEvent              ( QDropEvent *event ) override;
    void mousePressEvent        ( QMouseEvent *event ) override;
    void mouseDoubleClickEvent  ( QMouseEvent *event ) override;
    void mouseMoveEvent         ( QMouseEvent *event ) override;
    void mouseReleaseEvent      ( QMouseEvent *event ) override;
    void contextMenuEvent       ( QContextMenuEvent *event ) override;

    void initializeMenu();

    TP_Menu *menu_rightClick;
    QAction *action_remove;

    // previousItem & currentItem are only used in shuffle mode
    QListWidgetItem *previousItem, *nextItem;

    bool    b_isLeftButtonPressed;

    QString         qstr_keyword;
    bool            b_isFilenameSearched;
    bool            b_isAlbumSearched;
    bool            b_isArtistSearched;
    bool            b_isTitleSearched;
};

#endif // TP_FILELISTWIDGET_H
