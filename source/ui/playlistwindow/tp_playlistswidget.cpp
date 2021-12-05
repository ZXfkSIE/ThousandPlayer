#include "tp_playlistswidget.h"

#include "tp_globalconst.h"

#include "tp_filelistwidget.h"
#include "tp_menu.h"

#include <QAction>
#include <QMouseEvent>

TP_PlaylistsWidget::TP_PlaylistsWidget( QWidget *parent ) :
    QListWidget         { parent }
  , currentVisibleItem  { nullptr }
{
    setDragDropMode( QAbstractItemView::InternalMove );

    initializeMenu();
}


QListWidgetItem *
TP_PlaylistsWidget::addNewList( QString I_listName )
{
    auto *newItem       { new QListWidgetItem { I_listName } };
    newItem->setFlags( newItem->flags() | Qt::ItemIsEditable );

    auto *newFileList   { new TP_FileListWidget { nullptr } };      // The parent will be assigned within TP_PlaylistWindow later
    newItem->setData( TP::role_FileListAddress, QVariant::fromValue( newFileList ) );

    addItem( newItem );
    emit signal_fileListCreated( newFileList );

    if( count() == 1 )
        slot_switchVisibleFileList( newItem );

    return newItem;
}

// *****************************************************************
// private slots:
// *****************************************************************

void
TP_PlaylistsWidget::slot_removeCurrentItem()
{
    auto *itemToBeDeleted { currentItem() };
    if( itemToBeDeleted == currentVisibleItem )
    {
        auto idx_target { row( itemToBeDeleted ) };
        idx_target = idx_target == 0 ? 1 : idx_target - 1;
        slot_switchVisibleFileList( item( idx_target ) );
    }

    removeItemWidget( itemToBeDeleted );
    emit signal_fileListRemoved(
                itemToBeDeleted->data( TP::role_FileListAddress ).value< TP_FileListWidget * >()
                );
    delete itemToBeDeleted;
}


void
TP_PlaylistsWidget::slot_renameCurrentItem()
{
    editItem( currentItem() );
}


void
TP_PlaylistsWidget::slot_addItemFromContextMenu()
{
    editItem( addNewList( tr( "List " ) + QString::number( count() + 1 ) ) );
}


void
TP_PlaylistsWidget::slot_switchVisibleFileList( QListWidgetItem *I_item )
{
    if( currentVisibleItem == I_item )
        return;

    auto idx_target { row( I_item ) };

    for( unsigned i {}; i < count(); i++ )
    {
        auto *item_i { item( i ) };
        if( i == idx_target )
        {
            auto font { item_i->font() };
            font.setBold( true );
            item_i->setFont( font );
            item_i->setBackground( QColor( "#444" ) );
        }
        else
        {
            auto font { item_i->font() };
            font.setBold( false );
            item_i->setFont( font );
            item_i->setBackground( QColor( "#777" ) );
        }
    }

    emit signal_fileListSwitched( I_item->data( TP::role_FileListAddress ).value< TP_FileListWidget * >() );
    currentVisibleItem = I_item;
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_PlaylistsWidget::contextMenuEvent( QContextMenuEvent *event )
{
    auto *clickedItem = itemAt ( event->pos() );
    if( clickedItem )
    {
        setCurrentItem( clickedItem );
        action_remove->setEnabled( count() == 1 ? false : true );   // The last item cannot be removed
        action_rename->setEnabled( true );
    }
    else
    {
        action_remove->setEnabled( false );
        action_rename->setEnabled( false );
    }

    menu_rightClick->exec( event->globalPos() );
}


void
TP_PlaylistsWidget::mouseDoubleClickEvent( QMouseEvent *event )
{
    if( event->button() == Qt::LeftButton )
    {
        auto *clickedItem = itemAt ( event->pos() );
        if( clickedItem )
            slot_switchVisibleFileList( clickedItem );
    }
    else
        event->ignore();
}

// *****************************************************************
// private
// *****************************************************************

void
TP_PlaylistsWidget::initializeMenu()
{
    menu_rightClick = new TP_Menu { this };

    action_remove   = new QAction { tr( "&Remove" ), this };
    action_rename   = new QAction { tr( "Re&name" ), this };
    action_add      = new QAction { tr( "&Add" ), this };

    connect( action_remove, &QAction::triggered,
             this,          &TP_PlaylistsWidget::slot_removeCurrentItem );
    connect( action_rename, &QAction::triggered,
             this,          &TP_PlaylistsWidget::slot_renameCurrentItem );
    connect( action_add,    &QAction::triggered,
             this,          &TP_PlaylistsWidget::slot_addItemFromContextMenu );

    menu_rightClick->addAction( action_remove );
    menu_rightClick->addAction( action_rename );
    menu_rightClick->addSeparator();
    menu_rightClick->addAction( action_add );
}
