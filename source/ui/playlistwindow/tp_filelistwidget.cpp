﻿#include "tp_filelistwidget.h"

#include "tp_globalconst.h"
#include "tp_globalvariable.h"

#include "tp_menu.h"

#include <QFile>
#include <QMessageBox>
#include <QMouseEvent>

TP_FileListWidget::TP_FileListWidget( QWidget *parent, const QString &I_qstr ) :
    QListWidget     { parent }
  , previousItem    { nullptr }
  , nextItem        { nullptr }
  , listName        { I_qstr }
  , b_isConnected   { false }
{
    setMouseTracking( true );

    setDragDropMode( QAbstractItemView::InternalMove );
    setSelectionMode( QAbstractItemView::ExtendedSelection );

    QFont currentFont = font();
    currentFont.setPointSize( 10 );
    setFont( currentFont );

    setStyleSheet( "color: rgb(255, 255, 255);" );

    initializeMenu();
}


void
TP_FileListWidget::setListName( const QString &I_qstr )
{
    listName = I_qstr;
}


QString
TP_FileListWidget::getListName() const
{
    return listName;
}

void
TP_FileListWidget::setCurrentItemBold()
{
    if( TP::currentItem() == nullptr )
    {
        unsetCurrentItemBold();
        return;
    }

    const QUrl lURL = TP::currentItem()->data( TP::role_URL ).toUrl();
    for( size_t i {}; i < count(); i++ )
    {
        const QUrl rURL = item( i )->data( TP::role_URL ).toUrl();
        if( lURL == rURL )
        {
            QFont font = item( i )->font();
            item( i )->font();
            font.setBold( true );
            item( i )->setFont( font );
            item( i )->setBackground( QColor( "#444" ) );
        }
        else
        {
            QFont font = item( i )->font();
            item( i )->font();
            font.setBold( false );
            item( i )->setFont( font );
            item( i )->setBackground( QColor( "#777" ) );
        }
    }
}


void
TP_FileListWidget::unsetCurrentItemBold()
{
    for ( size_t i {}; i < count(); i++ )
    {
        QFont font = item( i )->font();
        font.setBold( false );
        item( i )->setFont( font );
        item( i )->setBackground( QColor("#777") );
    }
}


QListWidgetItem *
TP_FileListWidget::getCurrentItem()
{
    // No item in the list
    if ( count() == 0 )
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
    // No item in the list
    if ( count() == 0 )
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
    // No item in the list
    if ( count() == 0 )
        return nullptr;

    // Return the only item in the list
    if ( count() == 1 )
        return TP::currentItem() = item ( 0 );

    // Return the previous item
    int currentRow = indexFromItem( TP::currentItem() ).row();

    if( currentRow == 0 )
        return TP::currentItem() = item ( count() - 1 );
    else
        return TP::currentItem() = item ( currentRow - 1 );
}


QListWidgetItem *
TP_FileListWidget::getNextItem_shuffle()
{
    // No item in the list
    if ( count() == 0 )
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

    std::uniform_int_distribution<int> distribution { 0, count() - 1 } ;

    do
        randomRow = distribution( TP::randomEngine() );
    while( randomRow == currentRow );

    previousItem = TP::currentItem();
    return TP::currentItem() = item ( randomRow );
}

QListWidgetItem *
TP_FileListWidget::getPreviousItem_shuffle()
{
    // No item in the list
    if ( count() == 0 )
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

    std::uniform_int_distribution<int> distribution { 0, count() - 1 } ;

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
TP_FileListWidget::refreshShowingTitle( int idx_Min, const int idx_Max )
{
    if ( idx_Max < 0 )
        return;

    if ( idx_Min == -1 )
        idx_Min++;

    for ( int i {idx_Min}; i <= idx_Max; i++ )
        item(i)->setText(
                    QString("%1. ").arg(i + 1)
                    +
                    item(i)->data(TP::role_Description).toString() );
}


void
TP_FileListWidget::clearUnselectedItems()
{
    qsizetype numberOfSelectedItems { selectedItems().size() };

    // No item is selected or all items are selected
    if( ! numberOfSelectedItems
        ||
        numberOfSelectedItems == count() )
        return;

    reverseSelection();
    slot_clearSelectedItems();
}


void
TP_FileListWidget::clearInaccessibleItems()
{
    for( size_t i {}; i < count(); i++ )
    {
        switch ( item( i )->data( TP::role_SourceType ).value< TP::SourceType >() )
        {
        case TP::singleFile :
            if ( ! std::filesystem::exists(
                     item( i )->data( TP::role_URL ).toUrl().toLocalFile().toStdWString()
                     )
                 )
            {
                delete takeItem( i );
                i--;
            }
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
    if( numberOfSelectedItems == 0 )
        return;

    QMessageBox messageBox (
                QMessageBox::Warning,                           // QMessageBox::Icon icon
                tr( "Warning" ),                                // const QString &title
                tr( "Are you really want to delete"
                    "<br>%1"
                    "<br>from the <b>DISK</b>?"
                    "<br>NOTE: remote URLs will not be deleted." )
                .arg( numberOfSelectedItems == 1
                      ? QString( "\"%1\"" ).arg( selectedItems()[0]->data( TP::role_FileName ).toString() )
                      : tr( "these %1 items" ).arg( numberOfSelectedItems )
                      ),                                        // const QString &text
                QMessageBox::Yes | QMessageBox::No,             // QMessageBox::StandardButtons buttons
                this                                            // QWidget *parent
    );

    if( messageBox.exec() == QMessageBox::Yes )
    {
        size_t failureCount {};

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
                        this,                       // QWidget *parent
                        tr( "Warning" ),            // const QString &title
                        tr( "Failed to delete %1 items.\nAnyway, they have been removed from the list." )
                        .arg( failureCount )        // const QString &text
                        );

        refreshShowingTitle ( 0, count() - 1 );
    }
}


void
TP_FileListWidget::reverseSelection()
{
    for( size_t i {}; i < count(); i++ )
        item( i )->setSelected( ! item( i )->isSelected() );
}

void
TP_FileListWidget::sortByData( const int role, const bool isDescending )
{
    if( count() <= 1 )
        return;

    quickSort( role, 0, count() - 1, isDescending );

    refreshShowingTitle ( 0, count() - 1 );
}


// *****************************************************************
// public slots
// *****************************************************************


void
TP_FileListWidget::slot_clearSelectedItems()
{
    qsizetype numberOfSelectedItems { selectedItems().size() };

    // No item is selected
    if( numberOfSelectedItems == 0 )
        return;

    // All items are selected
    if( numberOfSelectedItems == count() )
    {
        clearAllItems();
        return;
    }

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

        delete takeItem( row( selectedItem ) );
    }

    refreshShowingTitle ( 0, count() - 1 );
}


// *****************************************************************
// private override
// *****************************************************************


void
TP_FileListWidget::dropEvent(QDropEvent *event)
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
TP_FileListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if ( b_isLeftButtonPressed )
        b_isLeftButtonPressed = false;

    QListWidget::mouseReleaseEvent( event );
}


// Right-click context menu
void
TP_FileListWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QListWidgetItem *selectedItem = itemAt ( event->pos() );
    if ( selectedItem == nullptr )
        return;

    setCurrentItem( selectedItem, QItemSelectionModel::Select );

    menu_rightClick->exec( event->globalPos() );
}


// *****************************************************************
// private
// *****************************************************************


void
TP_FileListWidget::initializeMenu()
{
    action_remove = new QAction { tr( "&Remove" ), this };

    connect(action_remove,  &QAction::triggered,
            this,           &TP_FileListWidget::slot_clearSelectedItems);

    menu_rightClick = new TP_Menu { this };
    menu_rightClick->addAction( action_remove );
}

// static const int role_Duration      = 0x0100;   // int
// static const int role_FileName      = 0x0107;   // QString
// static const int role_URL           = 0x0108;   // QUrl (compare the converted QString)
// static const int role_Description   = 0x010A;   // QString
void
TP_FileListWidget::quickSort( const int role, const int left, const int right, const bool isDescending )
{
    qDebug() << "[METHOD] TP_FileListWidget::quickSort(" << role << ", " << left << ", " << right << ")";
    if( left >= right )
        return;

    QListWidgetItem *pivot = item( left )->clone();
    int low = left, high = right;

    while( low < high )
    {
        switch( role )
        {
        case TP::role_Duration :
            if( ! isDescending )
                while( low < high &&
                      item( high )->data( role ).toInt() >= pivot->data( role ).toInt() )
                    high--;
            else
                while( low < high &&
                      item( high )->data( role ).toInt() <= pivot->data( role ).toInt() )
                    high--;

            break;

        case TP::role_FileName :
        case TP::role_Description :
        case TP::role_URL :
            if( ! isDescending )
                while( low < high &&
                       QString::compare(
                           item( high )->data( role ).toString(),
                           pivot->data( role ).toString(),
                           Qt::CaseInsensitive
                           ) >= 0
                       )
                    high--;
            else
                while( low < high &&
                       QString::compare(
                           item( high )->data( role ).toString(),
                           pivot->data( role ).toString(),
                           Qt::CaseInsensitive
                           ) <= 0
                       )
                    high--;

            break;
        }

        if( low < high )
        {                                       // item[low] = item[high]
            insertItem( low, item( high )->clone() );
            delete takeItem( low + 1 );
        }

        switch( role )
        {
        case TP::role_Duration :

            if( ! isDescending )
                while( low < high &&
                       item( low )->data( role ).toInt() < pivot->data( role ).toInt() )
                    low++;
            else
                while( low < high &&
                       item( low )->data( role ).toInt() > pivot->data( role ).toInt() )
                    low++;

            break;

        case TP::role_FileName :
        case TP::role_Description :
        case TP::role_URL :

            if( ! isDescending )
                while( low < high &&
                       QString::compare(
                           item( low )->data( role ).toString(),
                           pivot->data( role ).toString(),
                           Qt::CaseInsensitive
                           ) < 0
                       )
                    low++;
            else
                while( low < high &&
                       QString::compare(
                           item( low )->data( role ).toString(),
                           pivot->data( role ).toString(),
                           Qt::CaseInsensitive
                           ) > 0
                       )
                    low++;
            break;
        }

        if( low < high )
        {                                       // item[high] = item[low]
            delete takeItem( high );
            insertItem( high, item( low )->clone() );
        }
    }

    if( left < low )
    {                                           // itemp[low] = pivot
        delete takeItem( low );
        insertItem( low, pivot );
    }
    else
        delete pivot;

    quickSort( role, left, low - 1, isDescending );
    quickSort( role, low + 1, right, isDescending );
}
