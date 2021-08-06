q#include "tp_playlistwindow.h"
#include "ui_tp_playlistwindow.h"

#include "tp_globalconst.h"

#include "tp_filelisttablewidget.h"
#include "tp_menu.h"

// Headers of TagLib
#include <fileref.h>
#include <tag.h>

#include <QFileDialog>
#include <QMenu>
#include <QMouseEvent>

#include <filesystem>

#include <stdio.h>

TP_PlaylistWindow::TP_PlaylistWindow(QWidget *parent) :
    QWidget(parent)
  , ui( new Ui::TP_PlaylistWindow )
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);

    layout_FileListFrame = new QHBoxLayout {ui->frame_FileList};
    layout_FileListFrame->setContentsMargins(0, 0, 0, 0);

    ui->pushButton_Close->setIcon(QIcon{":/image/icon_Exit.svg"});

    initializePlaylist();
    initializeMenu();
}

TP_PlaylistWindow::~TP_PlaylistWindow()
{
    delete ui;

    storePlaylist();
}

// *****************************************************************
// private slots
// *****************************************************************

void
TP_PlaylistWindow::on_pushButton_Close_clicked()
{
    hide();
}

void TP_PlaylistWindow::on_action_AddFile_triggered()
{   
    QStringList qstrlst_FilePath = QFileDialog::getOpenFileNames(
                this,                               // QWidget *parent = nullptr
                tr("Open files"),                   // const QString &caption = QString()
                QString(),                          // const QString &dir = QString()
                QString("FLAC files (*.flac)")      // const QString &filter = QString(),
                );

    for (QString& qstr_FilePath: qstrlst_FilePath)
    {
        QFileInfo fileInfo { QFile { qstr_FilePath } };
        QString qstr_Filename = fileInfo.fileName();

        TagLib::FileRef fileRef { qstr_FilePath.toLocal8Bit().constData() };
        QString qstr_title = TStringToQString(fileRef.tag()->title());
        QString qstr_artist = TStringToQString(fileRef.tag()->artist());
        QString qstr_album = TStringToQString(fileRef.tag()->album());

        int duration = fileRef.audioProperties()->lengthInSeconds();

        currentFileListTableWidget->insertRow(currentFileListTableWidget->rowCount());

        // set duration
        currentFileListTableWidget->setItem(
                    currentFileListTableWidget->rowCount() - 1,
                    TP::index_Duration,
                    new QTableWidgetItem( QString("%1:%2")
                                          .arg(duration / 60, 2, 10, QChar('0'))
                                          .arg(duration % 60, 2, 10, QChar('0')) )
                 );

        // set path
        currentFileListTableWidget->setItem(
                    currentFileListTableWidget->rowCount() - 1,
                    TP::index_Path,
                    new QTableWidgetItem( qstr_FilePath )
                 );

        // set filename
        currentFileListTableWidget->setItem(
                    currentFileListTableWidget->rowCount() - 1,
                    TP::index_Filename,
                    new QTableWidgetItem( qstr_Filename )
                 );

        //set descrption, artist, title, album

        if(qstr_title.length() == 0)
            // No title in tag, meaning that no valid tag is contained in the file
            currentFileListTableWidget->setItem(
                        currentFileListTableWidget->rowCount() - 1,
                        TP::index_Description,
                        new QTableWidgetItem(qstr_Filename)
                     );
        else
            currentFileListTableWidget->setItem(
                        currentFileListTableWidget->rowCount() - 1,
                        TP::index_Description,
                        new QTableWidgetItem(qstr_artist + QString(" - ") + qstr_title) // be able to customized in the future
                    );


    }

    refreshNumber();
    refreshRowHeight();
    currentFileListTableWidget->resizeColumnToContents(TP::index_No);
    currentFileListTableWidget->resizeColumnToContents(TP::index_Duration);
}

// *****************************************************************
// private
// *****************************************************************

void
TP_PlaylistWindow::initializePlaylist()
{
    if( std::filesystem::exists(TP::playlistFilePath) )
    {
        qDebug() << "Existing playlist " << QString::fromStdString(TP::playlistFilePath) << " found.";
    }
    else
    {
        qDebug() << "Existing playlist not found. Creating default playlist and filelist.";
        ui->playlistsWidget->addItem(tr("Default"));
        ui->playlistsWidget->setCurrentRow(0);

        for(int i{}; i < 50; i++)
            ui->playlistsWidget->addItem(QString("L%1").arg(i));

        currentFileListTableWidget = new TP_FileListTableWidget{ ui->frame_FileList, tr("Default") };
        vector_FileListTableWidget.push_back(currentFileListTableWidget);
        layout_FileListFrame->addWidget(currentFileListTableWidget);
    }
}

void
TP_PlaylistWindow::initializeMenu()
{
    // Initialize menu of "Add" button
    menu_Add = new TP_Menu {ui->pushButton_Add};

    menu_Add->addAction(ui->action_AddFile);
    menu_Add->addAction(ui->action_AddFolder);
    menu_Add->addSeparator();
    menu_Add->addAction(ui->action_AddURL);

    ui->pushButton_Add->setMenu(menu_Add);
}

void
TP_PlaylistWindow::storePlaylist()
{
    if( !std::filesystem::exists(TP::configDirectoryPath) )
        std::filesystem::create_directory(TP::configDirectoryPath);
}

void
TP_PlaylistWindow::refreshNumber()
{
    for( size_t i {}; i < currentFileListTableWidget->rowCount(); i++ )
        if( currentFileListTableWidget->item(i, TP::index_No) )
        {
            currentFileListTableWidget->item(i, TP::index_No)->setText(QString::number(i + 1) + QString(". "));
        }
        else
        {
            currentFileListTableWidget->setItem( i, TP::index_No, new QTableWidgetItem( QString::number(i + 1) + QString(". ") ) );
        }
}

void
TP_PlaylistWindow::refreshRowHeight()
{
    for( size_t i {}; i < currentFileListTableWidget->rowCount(); i++ )
        currentFileListTableWidget->resizeRowToContents(i);
}
