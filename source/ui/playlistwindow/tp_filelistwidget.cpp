﻿#include "tp_filelistwidget.h"

#include "tp_globalconst.h"
#include "tp_globalvariable.h"

#include "tp_menu.h"

#include <QMouseEvent>

TP_FileListWidget::TP_FileListWidget(QWidget *parent, const QString &I_qstr) :
    QListWidget     { parent }
  , previousItem    { nullptr }
  , nextItem        { nullptr }
  , listName        { I_qstr }
  , b_isConnected   { false }
{
    setMouseTracking(true);

    setDragDropMode(QAbstractItemView::InternalMove);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    QFont currentFont = font();
    currentFont.setPointSize(10);
    setFont(currentFont);

    setStyleSheet("color: rgb(255, 255, 255);");

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

    const QUrl lURL = TP::currentItem()->data( TP::role_URL ).value<QUrl>();
    for( size_t i {}; i < count(); i++ )
    {
        const QUrl rURL = item( i )->data( TP::role_URL ).value<QUrl>();
        if( lURL == rURL )
        {
            QFont font = item( i )->font();
            item( i )->font();
            font.setBold( true );
            item( i )->setFont( font );
            item( i )->setBackground( QColor("#444") );
        }
        else
        {
            QFont font = item( i )->font();
            item( i )->font();
            font.setBold( false );
            item( i )->setFont( font );
            item( i )->setBackground( QColor("#777") );
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
TP_FileListWidget::modeIsNotShuffle()
{
    previousItem = nullptr;
    nextItem = nullptr;
}


// *****************************************************************
// public slots
// *****************************************************************

void
TP_FileListWidget::slot_refreshShowingTitle( int idx_Min, int idx_Max )
{
    if ( idx_Max < 0 )
        return;

    if ( idx_Min == -1 )
        idx_Min++;

    for ( int i {idx_Min}; i <= idx_Max; i++ )
        item(i)->setText(
                    QString("%1. ").arg(i + 1)
                    +
                    item(i)->data(TP::role_Description).value<QString>() );
}

// *****************************************************************
// private slots
// *****************************************************************

void
TP_FileListWidget::slot_clearAllItems()
{
    clear();
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_FileListWidget::slot_removeSelections()
{
    for( QListWidgetItem *selectedItem : selectedItems() )
    {
        if( previousItem == selectedItem )
            previousItem = nullptr;
        if( TP::currentItem() == selectedItem )
        {
            TP::currentItem() = nullptr;
            emit signal_currentItemUnset();
        }
        if( nextItem == selectedItem )
            nextItem = nullptr;

        delete takeItem( row(selectedItem) );
    }

    slot_refreshShowingTitle ( 0, count() - 1 );
}

void
TP_FileListWidget::dropEvent(QDropEvent *event)
{
    if ( b_isLeftButtonPressed )
        b_isLeftButtonPressed = false;

    QListWidget::dropEvent(event);
    slot_refreshShowingTitle ( 0, count() - 1 );
}

/* Disable all default list operations
 * (drag & drop, selection, double-click play, etc.)
 * triggered by buttons other than left button. */

void
TP_FileListWidget::mousePressEvent(QMouseEvent *event)
{
    if( event->button() == Qt::LeftButton )
    {
        b_isLeftButtonPressed = true;
        QListWidget::mousePressEvent(event);
    }
    else
        event->ignore();
}

void
TP_FileListWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if( event->button() == Qt::LeftButton )
        QListWidget::mouseDoubleClickEvent(event);
    else
        event->ignore();
}
void
TP_FileListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if ( b_isLeftButtonPressed )
        QListWidget::mouseMoveEvent(event);
    else
        event->ignore();
}

void
TP_FileListWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if ( b_isLeftButtonPressed )
        b_isLeftButtonPressed = false;

    QListWidget::mouseReleaseEvent(event);
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
    act_remove = new QAction { tr("&Remove"), this };

    connect(act_remove, &QAction::triggered,
            this,       &TP_FileListWidget::slot_removeSelections);

    menu_rightClick = new TP_Menu { this };
    menu_rightClick->addAction( act_remove );
}
