#include "tp_playlistswidget.h"

#include "tp_globalconst.h"
#include "tp_globalvariable.h"

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

void
TP_PlaylistsWidget::refreshFont()
{
    setFont( TP::config().getPlaylistFont() );

    for( int i {}; i < count(); i++ )
    {
        if( item( i ) == currentVisibleItem )
            setItemBold( item( i ) );
        else
            unsetItemBold( item( i ) );
    }
}


QListWidgetItem *
TP_PlaylistsWidget::addNewList( const QString &I_listName )
{
    auto *newItem {
        new QListWidgetItem { I_listName , this }
    };
    newItem->setFlags( newItem->flags() | Qt::ItemIsEditable );

    auto *newFileList { new TP_FileListWidget { nullptr } };  // The parent will be changed within TP_PlaylistWindow later

    newItem->setData( TP::role_FileListAddress,
                      QVariant::fromValue( reinterpret_cast< quintptr >( newFileList ) )
                      );
    newFileList->setFont( TP::config().getPlaylistFont() );

    addItem( newItem );
    emit signal_fileListCreated( newFileList );

    if( count() == 1 )
        switchVisibleFileList( newItem );

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
        currentVisibleItem = nullptr;
        auto idx_target { row( itemToBeDeleted ) };
        idx_target = ( ! idx_target ) ? 1 : idx_target - 1;
        switchVisibleFileList( item( idx_target ) );
    }

    removeItemWidget( itemToBeDeleted );
    emit signal_fileListRemoved(
                reinterpret_cast< TP_FileListWidget * >(
                    itemToBeDeleted->data( TP::role_FileListAddress ).value< quintptr >()
                    )
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
            switchVisibleFileList( clickedItem );
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


void
TP_PlaylistsWidget::switchVisibleFileList( QListWidgetItem *I_item )
{
    if( currentVisibleItem == I_item )
        return;

    if( currentVisibleItem )
        unsetItemBold( currentVisibleItem );

    currentVisibleItem = I_item;
    setItemBold( currentVisibleItem );

    emit signal_fileListSwitched(
                reinterpret_cast< TP_FileListWidget * >(
                    I_item->data( TP::role_FileListAddress ).value< quintptr >()
                    )
                );

}

void
TP_PlaylistsWidget::setItemBold( QListWidgetItem *I_item )
{
    auto font { TP::config().getPlaylistFont() };
    font.setBold( true );
    I_item->setFont( font );
    I_item->setBackground( QColor { "#444" } );
}


void
TP_PlaylistsWidget::unsetItemBold( QListWidgetItem *I_item )
{
    I_item->setFont( TP::config().getPlaylistFont() );
    I_item->setBackground( QColor { "#777" } );
}
