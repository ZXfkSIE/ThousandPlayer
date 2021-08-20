#include "tp_filelistwidget.h"

#include "tp_globalconst.h"

#include "tp_menu.h"

#include <QMouseEvent>

TP_FileListWidget::TP_FileListWidget(QWidget *parent, const QString &I_qstr) :
    QListWidget { parent }
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
TP_FileListWidget::setListName(const QString &I_qstr)
{
    listName = I_qstr;
}

QString
TP_FileListWidget::getListName()
{
    return listName;
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
