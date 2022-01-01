#include "tp_filelistwidget.h"

#include "tp_globalconst.h"
#include "tp_globalvariable.h"

#include "tp_menu.h"
#include "tp_progressdialog.h"

#include <QApplication>
#include <QFile>
#include <QMessageBox>
#include <QMouseEvent>

#include <stack>

TP_FileListWidget::TP_FileListWidget( QWidget *parent ) :
    QListWidget     { parent }
  , previousItem    {}
  , nextItem        {}
  , qstr_keyword    {}
{
    setMouseTracking( true );

    setDragDropMode( QAbstractItemView::InternalMove );
    setSelectionMode( QAbstractItemView::ExtendedSelection );

    setStyleSheet( "color: rgb(255, 255, 255);" );

    initializeMenu();
}

void
TP_FileListWidget::setCurrentItemBold()
{
    if( ! TP::currentItem() )
        return;

    auto font = TP::config().getPlaylistFont();
    font.setBold( true );
    TP::currentItem()->setFont( font );
    TP::currentItem()->setBackground( QColor { "#444" } );
}


void
TP_FileListWidget::unsetCurrentItemBold()
{
    if( ! TP::currentItem() )
        return;

    TP::currentItem()->setFont( TP::config().getPlaylistFont() );
    TP::currentItem()->setBackground( QColor { "#777" } );
}


QListWidgetItem *
TP_FileListWidget::getCurrentItem()
{
    unsetCurrentItemBold();

    // No item in the list
    if ( ! count() )
        return nullptr;

    // Return stored pointer
    if ( TP::currentItem() != nullptr )
        return TP::currentItem();

    // Return the only item in the list
    if ( count() == 1 )
        return TP::currentItem() = item ( 0 );

    // Return first selected item
    QList <QListWidgetItem *> selectedItemList = selectedItems();
    if( selectedItemList.count() != 0 )
        return TP::currentItem() = selectedItemList [0];

    // Return random item when in shuffle mode
    if( TP::config().getPlayMode() == TP::shuffle )
        return getNextItem_shuffle();

    // Return first item in the list
    return TP::currentItem() = item ( 0 );
}


QListWidgetItem *
TP_FileListWidget::getNextItem()
{
    unsetCurrentItemBold();

    // No item in the list
    if ( ! count() )
        return nullptr;

    // Return the only item in the list
    if ( count() == 1 )
        return TP::currentItem() = item ( 0 );

    // Return the next item
    int currentRow = indexFromItem( TP::currentItem() ).row();

    if( currentRow == count() - 1 )
        return TP::currentItem() = item ( 0 );
    else
        return TP::currentItem() = item ( currentRow + 1 );
}


QListWidgetItem *
TP_FileListWidget::getPreviousItem()
{
    unsetCurrentItemBold();

    // No item in the list
    if ( ! count() )
        return nullptr;

    // Return the only item in the list
    if ( count() == 1 )
        return TP::currentItem() = item ( 0 );

    // Return the previous item
    int currentRow = indexFromItem( TP::currentItem() ).row();

    if( ! currentRow )
        return TP::currentItem() = item ( count() - 1 );
    else
        return TP::currentItem() = item ( currentRow - 1 );
}


QListWidgetItem *
TP_FileListWidget::getNextItem_shuffle()
{
    unsetCurrentItemBold();

    // No item in the list
    if ( ! count() )
        return nullptr;

    // Return stored pointer
    if ( nextItem != nullptr )
    {
        previousItem = TP::currentItem();
        TP::currentItem() = nextItem;
        nextItem = nullptr;
        return TP::currentItem();
    }

    // Return the only item in the list
    if ( count() == 1 )
        return TP::currentItem() = item ( 0 );

    // Return a random item
    int currentRow;
    int randomRow { -1 } ;

    if( TP::currentItem() == nullptr )
        currentRow = -1;
    else
        currentRow = indexFromItem( TP::currentItem() ).row();

    std::uniform_int_distribution< int > distribution { 0, count() - 1 } ;

    do
        randomRow = distribution( TP::randomEngine() );
    while( randomRow == currentRow );

    previousItem = TP::currentItem();
    return TP::currentItem() = item ( randomRow );
}


QListWidgetItem *
TP_FileListWidget::getPreviousItem_shuffle()
{
    unsetCurrentItemBold();

    // No item in the list
    if ( ! count() )
        return nullptr;

    // Return stored pointer
    if ( previousItem != nullptr )
    {
        nextItem = TP::currentItem();
        TP::currentItem() = previousItem;
        previousItem = nullptr;
        return TP::currentItem();
    }

    // Return the only item in the list
    if ( count() == 1 )
        return TP::currentItem() = item ( 0 );

    // Return a random item
    int currentRow;
    int randomRow { -1 } ;

    if( TP::currentItem() == nullptr )
        currentRow = -1;
    else
        currentRow = indexFromItem( TP::currentItem() ).row();

    std::uniform_int_distribution< int > distribution { 0, count() - 1 } ;

    do
        randomRow = distribution( TP::randomEngine() );
    while( randomRow == currentRow );

    nextItem = TP::currentItem();
    return TP::currentItem() = item ( randomRow );
}


void
TP_FileListWidget::clearPreviousAndNext()
{
    previousItem = nullptr;
    nextItem = nullptr;
}


void
TP_FileListWidget::refreshShowingTitle( int I_minIdx, int I_maxIdx )
{
    if ( I_maxIdx < 0 )
        return;

    if ( I_minIdx == -1 )
        I_minIdx++;

    for ( int i { I_minIdx }; i <= I_maxIdx; i++ )
        item(i)->setText(
                    QString("%1. ").arg( i + 1 )
                    +
                    item( i )->data( TP::role_Description ).toString() );
}


void
TP_FileListWidget::clearUnselectedItems()
{
    auto numberOfSelectedItems { selectedItems().size() };

    // No item is selected or all items are selected
    if(     ! numberOfSelectedItems
        ||
            numberOfSelectedItems == count() )
        return;

    reverseSelection();
    slot_clearSelectedItems();
}


void
TP_FileListWidget::clearInaccessibleItems()
{
    for( int i {}; i < count(); i++ )
    {
        switch ( item( i )->data( TP::role_SourceType ).value< TP::SourceType >() )
        {
        case TP::singleFile :
            if ( ! std::filesystem::exists(
                     item( i )->data( TP::role_URL ).toUrl().toLocalFile().toStdWString()
                     )
                 )
                delete takeItem( i-- );

            break;

        default:
            break;
        }
    }

    refreshShowingTitle ( 0, count() - 1 );
}


void
TP_FileListWidget::clearAllItems()
{
    clear();

    previousItem = nullptr;
    nextItem = nullptr;
    TP::currentItem() = nullptr;
    emit signal_currentItemRemoved();
}


void
TP_FileListWidget::deleteSelectedItems()
{
    qsizetype numberOfSelectedItems { selectedItems().size() };

    // No item is selected
    if( ! numberOfSelectedItems )
        return;

    QMessageBox messageBox (
                QMessageBox::Warning,                           // QMessageBox::Icon icon
                tr( "Warning" ),                                // const QString &title
                tr( "Are you really want to delete"
                    "<br>%1"
                    "<br>from the <b>DISK</b>?"
                    "<br>NOTE: remote URLs cannot be deleted."
                    "<br>They will only be removed from the list.")
                .arg( numberOfSelectedItems == 1
                      ? QString( "\"%1\"" ).arg( selectedItems()[0]->data( TP::role_FileName ).toString() )
                      : tr( "these %1 items" ).arg( numberOfSelectedItems )
                      ),                                        // const QString &text
                QMessageBox::Yes | QMessageBox::No,             // QMessageBox::StandardButtons buttons
                this                                            // QWidget *parent
    );

    if( messageBox.exec() == QMessageBox::Yes )
    {
        unsigned failureCount {};

        for( QListWidgetItem *selectedItem : selectedItems() )
        {
            if( previousItem == selectedItem )
                previousItem = nullptr;
            if( TP::currentItem() == selectedItem )
            {
                TP::currentItem() = nullptr;
                emit signal_currentItemRemoved();
            }
            if( nextItem == selectedItem )
                nextItem = nullptr;

            TP::SourceType sourceType {
                static_cast<TP::SourceType>( selectedItem->data( TP::role_SourceType ).toInt() )
            };

            if( sourceType == TP::singleFile )
            {
                QFile qFile { selectedItem->data( TP::role_URL ).toUrl().toLocalFile() };

                if( ! qFile.moveToTrash() )
                    failureCount++;
            }

            delete takeItem( row( selectedItem ) );
        }

        if( failureCount )
            QMessageBox::warning(
                        this,
                        tr( "Warning" ),
                        tr( "Failed to delete %1 items.\nAnyway, they have been removed from the list." )
                        .arg( failureCount )
                        );

        refreshShowingTitle ( 0, count() - 1 );
    }
}


void
TP_FileListWidget::reverseSelection()
{
    for( unsigned i {}; i < count(); i++ )
        item( i )->setSelected( ! item( i )->isSelected() );
}


void
TP_FileListWidget::sortByData( int I_role, bool I_isDescending )
{
    if( count() <= 1 )
        return;

    int maxIdx { count() - 1 }, r {}, low {}, high {};
    std::uniform_int_distribution< int > distribution { 0, maxIdx } ;

    // Shuffle whole list to avoid worst situation
    for( unsigned i {}; i <= maxIdx; i++ )
    {
        r = distribution( TP::randomEngine() );

        // swap item( i ) & item( r )
        if( i < r )
        {
            insertItem( r, item( i )->clone() );            // item( i )->i,   item( r ) -> r+1
            insertItem( i, item( r + 1 )->clone() );        // item( i )->i+1, item( r ) -> r+2
            delete takeItem( r + 2 );
            delete takeItem( i + 1 );
        }
        else    // i > r
        {
            insertItem( i, item( r )->clone() );
            insertItem( r, item( i + 1 )->clone() );
            delete takeItem( i + 2 );
            delete takeItem( r + 1 );
        }
    }

    // Use own stack to prevent god damn stack overflow
    std::stack< std::pair< int, int > > stack {};
    stack.push( { 0, maxIdx } );

    QString qstr_Pivot {};

    while( true )
    {
        if( stack.empty() )
            break;

        auto [ left, right ] = stack.top();
        low = left, high = right;
        stack.pop();
        auto *pivot = item( left )->clone();

        while( low < high )
        {
            switch( I_role )
            {
            case TP::role_Duration :

                if( ! I_isDescending )
                    while( low < high &&
                          item( high )->data( I_role ).toInt() >= pivot->data( I_role ).toInt() )
                        high--;
                else
                    while( low < high &&
                          item( high )->data( I_role ).toInt() <= pivot->data( I_role ).toInt() )
                        high--;

                break;



            case TP::role_FileName :
            case TP::role_Description :
            case TP::role_URL :

                qstr_Pivot = pivot->data( I_role ).toString();

                if( ! I_isDescending )
                    while( low < high &&
                           QString::compare(
                               item( high )->data( I_role ).toString(), qstr_Pivot, Qt::CaseInsensitive
                               ) >= 0
                           )
                        high--;
                else
                    while( low < high &&
                           QString::compare(
                               item( high )->data( I_role ).toString(), qstr_Pivot, Qt::CaseInsensitive
                               ) <= 0
                           )
                        high--;

                break;



            case TP::role_Album :
            case TP::role_Artist :
            case TP::role_Title :

                qstr_Pivot = pivot->data( I_role ).toString().isEmpty() ?
                            pivot->data( TP::role_FileName ).toString() :
                            pivot->data( I_role ).toString();

                if( ! I_isDescending )
                {
                    while( low < high
                           &&
                           QString::compare(
                               item( high )->data( I_role ).toString().isEmpty() ?
                               item( high )->data( TP::role_FileName ).toString() :
                               item( high )->data( I_role ).toString(),
                               qstr_Pivot,
                               Qt::CaseInsensitive ) >= 0
                           )
                        high--;
                }
                else
                {
                    while( low < high
                           &&
                           QString::compare(
                               item( high )->data( I_role ).toString().isEmpty() ?
                               item( high )->data( TP::role_FileName ).toString() :
                               item( high )->data( I_role ).toString(),
                               qstr_Pivot,
                               Qt::CaseInsensitive ) <= 0
                           )
                        high--;
                }
                break;



            default:
                break;
            }

            if( low < high )
            {                                       // item[ low ] = item[ high ]
                insertItem( low, item( high )->clone() );
                delete takeItem( low + 1 );
                // Now it is certain that item[ low ] < pivot (when in ascending order),
                // thus item[ low ] can be jumped.
                low++;
            }

            switch( I_role )
            {
            case TP::role_Duration :

                if( ! I_isDescending )
                    while( low < high &&
                           item( low )->data( I_role ).toInt() < pivot->data( I_role ).toInt() )
                        low++;
                else
                    while( low < high &&
                           item( low )->data( I_role ).toInt() > pivot->data( I_role ).toInt() )
                        low++;

                break;



            case TP::role_FileName :
            case TP::role_Description :
            case TP::role_URL :

                if( ! I_isDescending )
                    while( low < high &&
                           QString::compare(
                               item( low )->data( I_role ).toString(),
                               qstr_Pivot,
                               Qt::CaseInsensitive
                               ) < 0
                           )
                        low++;
                else
                    while( low < high &&
                           QString::compare(
                               item( low )->data( I_role ).toString(),
                               qstr_Pivot,
                               Qt::CaseInsensitive
                               ) > 0
                           )
                        low++;

                break;



            case TP::role_Album :
            case TP::role_Artist :
            case TP::role_Title :

                if( ! I_isDescending )
                    while( low < high
                           &&
                           QString::compare(
                               item( low )->data( I_role ).toString().isEmpty() ?
                               item( low )->data( TP::role_FileName ).toString() :
                               item( low )->data( I_role ).toString(),
                               qstr_Pivot,
                               Qt::CaseInsensitive ) < 0
                           )
                        low++;
                else
                    while( low < high
                           &&
                           QString::compare(
                               item( low )->data( I_role ).toString().isEmpty() ?
                               item( low )->data( TP::role_FileName ).toString() :
                               item( low )->data( I_role ).toString(),
                               qstr_Pivot,
                               Qt::CaseInsensitive ) > 0
                           )
                        low++;

                break;



            default:
                break;
            }

            if( low < high )
            {                                       // item[ high ] = item[ low ]
                delete takeItem( high );
                insertItem( high, item( low )->clone() );
                // Now it is certain that item[ high ] >= pivot (when in ascending order),
                // thus item[ high ] can be jumped.
                high--;
            }
        }       // while( low < high )

        if( left < low )
        {                                           // item[ low ] = pivot
            delete takeItem( low );
            insertItem( low, pivot );
        }
        else
            delete pivot;

        if( low - 1 - left < right - low - 1 )      // smaller range first
        {
            if( low + 1 < right )
                stack.push( { low + 1, right } );
            if( left < low - 1 )
                stack.push( { left, low - 1 } );
        }
        else
        {
            if( left < low - 1 )
                stack.push( { left, low - 1 } );
            if( low + 1 < right )
                stack.push( { low + 1, right } );
        }

    }   // while( true )

    refreshShowingTitle ( 0, count() - 1 );
}


void
TP_FileListWidget::searchByData( const QString &  I_qstr_keyword,
                                 qsizetype        I_startingIdx,
                                 bool             I_isFilenameSearched,
                                 bool             I_isAlbumSearched,
                                 bool             I_isArtistSearched,
                                 bool             I_isTitleSearched )
{
    if( ! I_qstr_keyword.size() || ! count() )
        return;

    clearSelection();

    qstr_keyword            = I_qstr_keyword;
    b_isFilenameSearched    = I_isFilenameSearched;
    b_isAlbumSearched       = I_isAlbumSearched;
    b_isArtistSearched      = I_isArtistSearched;
    b_isTitleSearched       = I_isTitleSearched;

    for( auto i { I_startingIdx }; i < count(); i++ )
    {
        auto *currentItem = item( i );

        if(     I_isFilenameSearched &&
                currentItem->data( TP::role_FileName ).toString().contains( qstr_keyword, Qt::CaseInsensitive )
            ||
                I_isAlbumSearched &&
                currentItem->data( TP::role_Album ).toString().contains( qstr_keyword, Qt::CaseInsensitive )
            ||
                I_isArtistSearched &&
                currentItem->data( TP::role_Artist ).toString().contains( qstr_keyword, Qt::CaseInsensitive )
        )
        {
            setCurrentItem( currentItem );
            return;
        }

        // If the audio doesn't contain a valid title, search filename instead.
        if( I_isTitleSearched )
        {
            const auto &title { currentItem->data( TP::role_Title ).toString() };
            if(     title.size() && title.contains( qstr_keyword, Qt::CaseInsensitive )
                ||
                    ! I_isFilenameSearched &&
                    currentItem->data( TP::role_FileName ).toString().contains( qstr_keyword, Qt::CaseInsensitive )
            )
            {
                setCurrentItem( currentItem );
                return;
            }
        }
    }

    QMessageBox::warning(
                this,
                tr( "Warning" ),
                tr( "No result found." )
                );
}


void
TP_FileListWidget::findNext()
{
    if( ! qstr_keyword.size() || ! count() )
        return;

    if( currentItem() )
        searchByData( qstr_keyword,
                      row( currentItem() ) + 1,
                      b_isFilenameSearched,
                      b_isAlbumSearched,
                      b_isArtistSearched,
                      b_isTitleSearched );
    else
        searchByData( qstr_keyword,
                      0,
                      b_isFilenameSearched,
                      b_isAlbumSearched,
                      b_isArtistSearched,
                      b_isTitleSearched );
}


// *****************************************************************
// public slots
// *****************************************************************


void
TP_FileListWidget::slot_clearSelectedItems()
{
    auto numberOfSelectedItems { selectedItems().size() };

    // No item is selected
    if( ! numberOfSelectedItems )
        return;

    // All items are selected
    if( numberOfSelectedItems == count() )
    {
        clearAllItems();
        return;
    }

    for( auto *selectedItem : selectedItems() )
    {
        if( previousItem == selectedItem )
            previousItem = nullptr;
        if( TP::currentItem() == selectedItem )
        {
            TP::currentItem() = nullptr;
            emit signal_currentItemRemoved();
        }
        if( nextItem == selectedItem )
            nextItem = nullptr;

        delete takeItem( row( selectedItem ) );
    }

    refreshShowingTitle ( 0, count() - 1 );
}


// *****************************************************************
// private override
// *****************************************************************


void
TP_FileListWidget::dropEvent( QDropEvent *event )
{
    if ( b_isLeftButtonPressed )
        b_isLeftButtonPressed = false;

    QListWidget::dropEvent( event );
    refreshShowingTitle ( 0, count() - 1 );
}

/* Disable all default list operations
 * (drag & drop, selection, double-click play, etc.)
 * triggered by buttons other than left button. */

void
TP_FileListWidget::mousePressEvent( QMouseEvent *event )
{
    if( event->button() == Qt::LeftButton )
    {
        b_isLeftButtonPressed = true;
        QListWidget::mousePressEvent( event );
    }
    else
        event->ignore();
}


void
TP_FileListWidget::mouseDoubleClickEvent( QMouseEvent *event )
{
    if( event->button() == Qt::LeftButton )
        QListWidget::mouseDoubleClickEvent( event );
    else
        event->ignore();
}


void
TP_FileListWidget::mouseMoveEvent( QMouseEvent *event )
{
    if ( b_isLeftButtonPressed )
        QListWidget::mouseMoveEvent( event );
    else
        event->ignore();
}


void
TP_FileListWidget::mouseReleaseEvent( QMouseEvent *event )
{
    if ( b_isLeftButtonPressed )
        b_isLeftButtonPressed = false;

    QListWidget::mouseReleaseEvent( event );
}


// Right-click context menu
void
TP_FileListWidget::contextMenuEvent( QContextMenuEvent *event )
{
    auto *clickedItem = itemAt ( event->pos() );

    if ( clickedItem == nullptr )
        return;

    if ( ! clickedItem->isSelected() )
        clearSelection();

    setCurrentItem( clickedItem, QItemSelectionModel::Select );

    menu_rightClick->exec( event->globalPos() );
}


// *****************************************************************
// private
// *****************************************************************


void
TP_FileListWidget::initializeMenu()
{
    menu_rightClick = new TP_Menu { this };

    action_remove = new QAction { tr( "&Remove" ), this };

    connect( action_remove, &QAction::triggered,
             this,          &TP_FileListWidget::slot_clearSelectedItems );

    menu_rightClick->addAction( action_remove );
}
