#include "tp_playlistwindow.h"
#include "ui_tp_playlistwindow.h"

#include "tp_globalconst.h"
#include "tp_globalfunction.h"
#include "tp_globalvariable.h"

#include "tp_filelistwidget.h"
#include "tp_menu.h"

#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>

TP_PlaylistWindow::TP_PlaylistWindow( QWidget *parent ) :
    QWidget                 { parent }
  , ui                      { new Ui::TP_PlaylistWindow }
  , currentFileListWidget   {}
  , vector_FileListWidget   {}
{
    ui->setupUi( this );
    // Qt::Tool is used for getting rid of the window tab in taskbar
    setWindowFlags( windowFlags() | Qt::FramelessWindowHint | Qt::Tool | Qt::NoDropShadowWindowHint );

    initializeConnection();

    layout_FileListFrame = new QHBoxLayout { ui->frame_FileList };
    layout_FileListFrame->setContentsMargins( 0, 0, 0, 0 );

    ui->pushButton_Close->setIcon( QIcon { ":/image/icon_Exit.svg" } );

    initializeMenu();

    /****************************** WARNING *****************************************
     *  The method initializePlaylist()
     *  need to be executed manually after TP_MainClass initialized the connections.
     ********************************************************************************/
}

TP_PlaylistWindow::~TP_PlaylistWindow()
{
    delete ui;

    storePlaylist();
}


void
TP_PlaylistWindow::initializePlaylist()
{
    if( std::filesystem::exists( TP::playlistFilePath.toStdWString() ) )
    {
        qDebug() << "Existing playlist " << TP::playlistFilePath << " found.";
    }
    else
    {
        qDebug() << "Existing playlist not found. Creating default playlist and filelist.";
        ui->playlistsWidget->addItem( tr( "Default" ) );
        ui->playlistsWidget->setCurrentRow( 0 );

        currentFileListWidget = new TP_FileListWidget{ ui->frame_FileList, tr( "Default" ) };
        vector_FileListWidget.push_back( currentFileListWidget );
        layout_FileListFrame->addWidget( currentFileListWidget );

        qDebug() << "[SIGNAL] signal_NewFilelistWidgetCreated -- list's name is " << currentFileListWidget->getListName();

        // When current item is deleted
        connect(currentFileListWidget,  &TP_FileListWidget::signal_currentItemRemoved,
                this,                   &TP_PlaylistWindow::slot_currentItemRemoved);

        emit signal_newFilelistWidgetCreated( currentFileListWidget );
    }
}


void
TP_PlaylistWindow::setCurrentItem( QListWidgetItem * I_item )
{
    currentFileListWidget->setCurrentItem( I_item );
}


void
TP_PlaylistWindow::setCurrentItemBold()
{
    currentFileListWidget->setCurrentItemBold();
}


void
TP_PlaylistWindow::unsetCurrentItemBold()
{
    currentFileListWidget->unsetCurrentItemBold();
}



void
TP_PlaylistWindow::refreshShowingTitle( QListWidgetItem *I_item )
{
    int index {
        currentFileListWidget->indexFromItem( TP::currentItem() ).row()
    };
    currentFileListWidget->refreshShowingTitle( index, index );

}


QListWidgetItem *
TP_PlaylistWindow::getCurrentItem()
{
    return currentFileListWidget->getCurrentItem();
}


QListWidgetItem *
TP_PlaylistWindow::getNextItem()
{
    return currentFileListWidget->getNextItem();
}


QListWidgetItem *
TP_PlaylistWindow::getPreviousItem()
{
    return currentFileListWidget->getPreviousItem();
}


QListWidgetItem *
TP_PlaylistWindow::getNextItem_shuffle()
{
    return currentFileListWidget->getNextItem_shuffle();
}


QListWidgetItem *
TP_PlaylistWindow::getPreviousItem_shuffle()
{
    return currentFileListWidget->getPreviousItem_shuffle();
}


// *****************************************************************
// public slots
// *****************************************************************

void
TP_PlaylistWindow::slot_clearPreviousAndNext()
{
    currentFileListWidget->clearPreviousAndNext();
}


// *****************************************************************
// private slots
// *****************************************************************


void
TP_PlaylistWindow::slot_moveTitleBar( QRect newGeometry )
{
    emit signal_moveWindow( this, newGeometry );
}


void
TP_PlaylistWindow::slot_leftButtonReleased()
{
    emit signal_leftButtonReleased();
}


void
TP_PlaylistWindow::slot_resizeWindow( QRect newGeomtry, TP::ResizeType resizeType )
{
    emit signal_resizeWindow( this, newGeomtry, resizeType );
}


void
TP_PlaylistWindow::slot_currentItemRemoved()
{
    qDebug() << "[SIGNAL] TP_PlaylistWindow::signal_currentItemRemoved";
    emit signal_currentItemRemoved();
}


void
TP_PlaylistWindow::on_pushButton_Close_clicked()
{
    hide();
}


void TP_PlaylistWindow::on_action_addFiles_triggered()
{
    int originalCount { currentFileListWidget->count() };

    QList <QUrl> fileURLs = QFileDialog::getOpenFileUrls(
                this,                               // QWidget *parent = nullptr
                tr("Open files"),                   // const QString &caption = QString()
                QString(),                          // const QString &dir = QString()
                QString("FLAC files (*.flac);;"     // const QString &filter = QString()
                        "MP3 files (*.mp3)")
                );

    for (const QUrl& fileURL: fileURLs)
    {
        QListWidgetItem *item = new QListWidgetItem { currentFileListWidget };
        item->setData( TP::role_URL, fileURL );         // set URL
        TP::storeInformation( item );
        currentFileListWidget->addItem( item );
    }

    currentFileListWidget->refreshShowingTitle( originalCount - 1, currentFileListWidget->count() - 1 );
}


void
TP_PlaylistWindow::on_action_clearSelectedItems_triggered()
{
    currentFileListWidget->slot_clearSelectedItems();
}


void
TP_PlaylistWindow::on_action_clearUnselectedItems_triggered()
{
    currentFileListWidget->clearUnselectedItems();
}


void
TP_PlaylistWindow::on_action_clearAllItems_triggered()
{
    currentFileListWidget->clearAllItems();
}


void
TP_PlaylistWindow::on_action_clearInaccessibleItems_triggered()
{
    currentFileListWidget->clearInaccessibleItems();
}


void
TP_PlaylistWindow::on_action_deleteFromDisk_triggered()
{
    currentFileListWidget->deleteSelectedItems();
}


void
TP_PlaylistWindow::on_action_selectAll_triggered()
{
    currentFileListWidget->selectAll();
}


void
TP_PlaylistWindow::on_action_unselectAll_triggered()
{
    currentFileListWidget->clearSelection();
}


void
TP_PlaylistWindow::on_action_reverseSelection_triggered()
{
    currentFileListWidget->reverseSelection();
}

// *****************************************************************
// private override
// *****************************************************************


void
TP_PlaylistWindow::showEvent( QShowEvent *event )
{
    QWidget::showEvent( event );
    emit signal_shown();
}


void
TP_PlaylistWindow::hideEvent( QHideEvent *event )
{
    QWidget::hideEvent( event );
    emit signal_hidden();
}


// *****************************************************************
// private
// *****************************************************************


void
TP_PlaylistWindow::initializeMenu()
{
    // =============== Initialize menu of "Add" button ===============
    menu_Add = new TP_Menu { ui->pushButton_Add };

    menu_Add->addAction( ui->action_addFiles );
    /* ----- Pending implementation ----- */
    // menu_Add->addAction( ui->action_addFolder );
    // menu_Add->addSeparator();
    // menu_Add->addAction( ui->action_addURL );

    ui->pushButton_Add->setMenu( menu_Add );

    // =============== Initialize menu of "Remove" button ===============
    menu_Remove = new TP_Menu { ui->pushButton_Remove };

    menu_Remove->addAction( ui->action_clearSelectedItems );
    menu_Remove->addAction( ui->action_clearUnselectedItems );
    menu_Remove->addSeparator();
    menu_Remove->addAction( ui->action_clearInaccessibleItems );
    menu_Remove->addAction( ui->action_clearAllItems );
    menu_Remove->addSeparator();
    menu_Remove->addAction( ui->action_deleteFromDisk );

    ui->pushButton_Remove->setMenu( menu_Remove );

    // =============== Initialize menu of "Select" button ===============
    menu_Select = new TP_Menu { ui->pushButton_Select };

    menu_Select->addAction( ui->action_selectAll );
    menu_Select->addAction( ui->action_unselectAll );
    menu_Select->addSeparator();
    menu_Select->addAction( ui->action_reverseSelection );

    ui->pushButton_Select->setMenu( menu_Select );
}


void
TP_PlaylistWindow::initializeConnection()
{
    // Window moving and resizing related
    connect(ui->frame_Title,        &TP_TitleBar::signal_moveTitleBar,
            this,                   &TP_PlaylistWindow::slot_moveTitleBar);
    connect(ui->frame_Title,        &TP_TitleBar::signal_leftButtonReleased,
            this,                   &TP_PlaylistWindow::slot_leftButtonReleased);
    connect(ui->playlistContainer,  &TP_PlaylistContainer::signal_resizeWindow,
            this,                   &TP_PlaylistWindow::slot_resizeWindow);
    connect(ui->playlistContainer,  &TP_PlaylistContainer::signal_leftButtonReleased,
            this,                   &TP_PlaylistWindow::slot_leftButtonReleased);
    connect(ui->frame_Bottom,       &TP_PlaylistBottomFrame::signal_resizeWindow,
            this,                   &TP_PlaylistWindow::slot_resizeWindow);
    connect(ui->frame_Bottom,       &TP_PlaylistBottomFrame::signal_leftButtonReleased,
            this,                   &TP_PlaylistWindow::slot_leftButtonReleased);
}


void
TP_PlaylistWindow::storePlaylist()
{
    if( ! std::filesystem::exists( TP::configDirectoryPath.toStdWString() ) )
        std::filesystem::create_directory( TP::configDirectoryPath.toStdWString() );
}
