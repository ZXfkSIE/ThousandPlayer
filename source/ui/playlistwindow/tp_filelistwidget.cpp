#include "tp_filelistwidget.h"

#include "tp_globalconst.h"
#include "tp_globalvariable.h"

#include "tp_menu.h"

#include <QMouseEvent>

TP_FileListWidget::TP_FileListWidget(QWidget *parent, const QString &I_qstr) :
    QListWidget { parent }
  , previousItem { nullptr }
  , currentItem { nullptr }
  , nextItem { nullptr }
  , listName { I_qstr }
  , b_isConnected { false }
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
TP_FileListWidget::setCurrentItem( QListWidgetItem * I_item )
{
    currentItem = I_item;

    QUrl lURL = I_item->data( TP::role_URL ).value<QUrl>();
    for( size_t i {}; i < count(); i++ )
    {
        QUrl rURL = item( i )->data( TP::role_URL ).value<QUrl>();
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

QListWidgetItem *
TP_FileListWidget::getCurrentItem()
{
    // No item
    if ( count() == 0 )
        return nullptr;

    // Return stored pointer
    if ( currentItem != nullptr )
        return currentItem;

    // Return first selected item
    QList <QListWidgetItem *> selectedItemList = selectedItems();
    if( selectedItemList.count() != 0 )
        return selectedItemList [0];

    // Return random item when in shuffle mode
    if( TP::Config().getPlayMode() == TP::shuffle )
        return getNextItem();

    // Return first item in the list
    return item(0);
}


QListWidgetItem *
TP_FileListWidget::getNextItem()
{
    if ( count() == 0 )
        return nullptr;

    switch ( TP::Config().getPlayMode() )
    {
    case TP::singleTime :

    }
}

QListWidgetItem *
TP_FileListWidget::getPreviousItem()
{
    if ( currentFileListWidget->count() == 0 )
        return nullptr;

}

// *****************************************************************
// public slots:
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
                    qvariant_cast<QString>( item(i)->data(TP::role_Description)) );
}

// *****************************************************************
// private slots:
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
        delete takeItem( row(selectedItem) );

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
