#include "tp_lyricseditor.h"
#include "ui_tp_lyricseditor.h"

#include "tp_globalconst.h"
#include "tp_globalfunction.h"
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


void
TP_LyricsEditor::refreshFont()
{
    ui->plainTextEdit->setFont( TP::config().getLyricsFont() );
}

void
TP_LyricsEditor::readLyricsFile( const QUrl &I_url )
{
    if( I_url.isEmpty() )
        return;

    TP::config().setLastOpenedDirectory( I_url.adjusted( QUrl::RemoveFilename ) );

    QFile qFile { I_url.toLocalFile() };
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

    currentFileURL = I_url;
}

// *****************************************************************
// private slots:
// *****************************************************************

void
TP_LyricsEditor::on_pushButton_Return_clicked()
{
    if( ui->plainTextEdit->document()->isModified() )
    {
        switch ( QMessageBox::question(
                     this,                                                      // QWidget *parent
                     tr( "Warning" ),                                           // const QString &title
                     tr( "Current lyrics file has not been saved. Save it?" ),  // const QString &text
                     QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,  // QMessageBox::StandardButtons buttons
                     QMessageBox::Cancel                                        // QMessageBox::StandardButton defaultButton
                     ) )
        {
        case QMessageBox::Yes :
            if( openSaveFileDialog() )
                returnToLyricsViewer( currentFileURL );
            break;

        case QMessageBox::No :
            returnToLyricsViewer( {} );
            break;

        default :
            break;
        }
    }
    else
        returnToLyricsViewer( {} );
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

    readLyricsFile( fileURL );
}


void
TP_LyricsEditor::on_pushButton_Save_clicked()
{
    openSaveFileDialog();
}


void
TP_LyricsEditor::on_pushButton_InsertTimestamp_clicked()
{
    if( currentPosition < 0 )
        return;

    auto m { currentPosition };
    auto ms { m % 1000 };
    m /= 1000;
    auto s { m % 60 };
    m /= 60;

    ui->plainTextEdit->moveCursor( QTextCursor::StartOfLine );
    ui->plainTextEdit->insertPlainText(
                QString { "[%1:%2.%3]" }
                .arg( m, 2, 10, QLatin1Char { '0' } )
                .arg( s, 2, 10, QLatin1Char { '0' } )
                .arg( ms, 3, 10, QLatin1Char { '0' } )
                );
    ui->plainTextEdit->moveCursor( QTextCursor::Down );
    ui->plainTextEdit->moveCursor( QTextCursor::StartOfLine );
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


void
TP_LyricsEditor::returnToLyricsViewer( const QUrl &I_url )
{
    currentFileURL.clear();
    ui->plainTextEdit->clear();
    emit signal_switchToLyricsViewer( I_url );
}


bool
TP_LyricsEditor::openSaveFileDialog()
{
    QUrl savingTargetURL {};

    if( ! currentFileURL.isEmpty() )
        // currentFileURL exists
    {
        savingTargetURL = currentFileURL;
    }
    else
    {
        savingTargetURL = TP::getLyricsURL( TP::currentItem() );    // currentFileURL does not exist, but there is a file being played
        if( savingTargetURL.isEmpty() )                             // Cannot get default saving target anywhere
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
        return false;

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
            return false;
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
        return false;
    }

    currentFileURL = fileURL;
    return true;
}
