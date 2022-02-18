#include "tp_lyricseditor.h"
#include "ui_tp_lyricseditor.h"

#include "tp_globalconst.h"
#include "tp_globalvariable.h"

#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QSaveFile>

TP_LyricsEditor::TP_LyricsEditor( QWidget *parent ) :
    QWidget         { parent }
  , ui              { new Ui::TP_LyricsEditor }
  , currentFileURL  {}
  , currentPosition {}
{
    ui->setupUi( this );

    initializeUI();
}


TP_LyricsEditor::~TP_LyricsEditor()
{
    delete ui;
}


void
TP_LyricsEditor::setCurrentPosition( qint64 I_ms )
{
    currentPosition = I_ms;
}

// *****************************************************************
// public slots:
// *****************************************************************

void
TP_LyricsEditor::slot_readLyricsFile( const QUrl &I_URL )
{
    if( I_URL.isEmpty() )
        return;

    TP::config().setLastOpenedDirectory( I_URL.adjusted( QUrl::RemoveFilename ) );

    QFile qFile { I_URL.toLocalFile() };
    if( ! qFile.open( QIODeviceBase::ReadOnly | QIODeviceBase::Text ) )
    {
        QMessageBox::critical(
                    this,
                    tr( "Error" ),
                    tr( "Could not open %1 :\n" ).arg( qFile.fileName() ) + qFile.errorString()
                    );
        return;
    }

    ui->plainTextEdit->setPlainText( qFile.readAll() );
    ui->plainTextEdit->document()->setModified( false );
    ui->plainTextEdit->moveCursor( QTextCursor::Start );
    ui->plainTextEdit->ensureCursorVisible();

    currentFileURL = I_URL;
}

// *****************************************************************
// private slots:
// *****************************************************************

void
TP_LyricsEditor::on_pushButton_Return_clicked()
{

    currentFileURL.clear();
}


void
TP_LyricsEditor::on_pushButton_Open_clicked()
{
    const auto &fileURL {
        QFileDialog::getOpenFileUrl(
                    this,                                           // QWidget *parent = nullptr
                    tr( "Open Lyrics File" ),                       // const QString &caption = QString()
                    TP::config().getLastOpenedDirectory(),          // const QUrl &dir = QUrl()
                    tr( "Lyrics files" ) + QString{ " (*.lrc)" }    // const QString &filter = QString()
                    )
    };

    slot_readLyricsFile( fileURL );
}


void
TP_LyricsEditor::on_pushButton_Save_clicked()
{
    QUrl savingTargetURL {};

    if( ! currentFileURL.isEmpty() )
        // currentFileURL exists
    {
        savingTargetURL = currentFileURL;
    }
    else if( TP::currentItem() &&
             TP::currentItem()->data( TP::role_SourceType ).value< TP::SourceType >() == TP::singleFile )
        // currentFileURL does not exist, but there is a file being played
    {
        auto qstr_FileName { TP::currentItem()->data( TP::role_FileName ).toString() };
        qstr_FileName = qstr_FileName.left( qstr_FileName.lastIndexOf( '.' ) ) + QString { ".lrc" };
        savingTargetURL = {
            TP::currentItem()->data( TP::role_URL ).toUrl().adjusted( QUrl::RemoveFilename ).toString()
            + qstr_FileName
        };
    }
    else
        // Cannot get default saving target anywhere
    {
        savingTargetURL = TP::config().getLastOpenedDirectory();
    }

    const auto &fileURL {
        QFileDialog::getSaveFileUrl(
                    this,                                           // QWidget *parent = nullptr
                    tr( "Save Lyrics File" ),                       // const QString &caption = QString()
                    savingTargetURL,                                 // const QUrl &dir = QUrl()
                    tr( "Lyrics files" ) + QString { " (*.lrc)" }   // const QString &filter = QString()
                    )
    };

    if( fileURL.isEmpty() )
        return;

    TP::config().setLastOpenedDirectory( fileURL.adjusted( QUrl::RemoveFilename ) );

    QSaveFile savingFile { fileURL.toLocalFile() };
    if( savingFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text ) )
    {
        QTextStream outTextStream { &savingFile };
        outTextStream << ui->plainTextEdit->toPlainText();
        if( ! savingFile.commit() )
        {
            QMessageBox::critical(
                        this,
                        tr( "Error" ),
                        tr( "Could not write %1 :\n" ).arg( savingFile.fileName() )
                        + savingFile.errorString()
                        );
            return;
        }
    }
    else
    {
        QMessageBox::critical(
                    this,
                    tr( "Error" ),
                    tr( "Could not open %1 for writing:\n" ).arg( savingFile.fileName() )
                    + savingFile.errorString()
                    );
        return;
    }

    currentFileURL = fileURL;
}


void
TP_LyricsEditor::on_pushButton_InsertTimestamp_clicked()
{

}

// *****************************************************************
// private
// *****************************************************************

void
TP_LyricsEditor::initializeUI()
{
    ui->pushButton_Return   ->setIcon( QIcon{ ":/image/icon_ReturnToLyricsViewer.svg" } );
    ui->pushButton_Open     ->setIcon( QIcon{ ":/image/icon_OpenLyricsFile.svg" } );
    ui->pushButton_Save     ->setIcon( QIcon{ ":/image/icon_SaveLyricsFile.svg" } );
}
