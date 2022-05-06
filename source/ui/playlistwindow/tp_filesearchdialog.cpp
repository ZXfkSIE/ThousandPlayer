#include "tp_filesearchdialog.h"
#include "ui_tp_filesearchdialog.h"

#include <QMessageBox>

TP_FileSearchDialog::TP_FileSearchDialog( QWidget *parent ) :
    QDialog { parent }
  , ui      { new Ui::TP_FileSearchDialog }
{
    ui->setupUi( this );

    setWindowIcon( QIcon{ ":/image/icon_FileSearchDialog.svg" } );
}


TP_FileSearchDialog::~TP_FileSearchDialog()
{
    delete ui;
}


QString
TP_FileSearchDialog::getKeyword() const
{
    return qstr_keyword;
}


bool
TP_FileSearchDialog::isFilenameSearched() const
{
    return b_isFilenameSearched;
}


bool
TP_FileSearchDialog::isAlbumSearched() const
{
    return b_isAlbumSearched;
}


bool
TP_FileSearchDialog::isArtistSearched() const
{
    return b_isArtistSearched;
}

bool
TP_FileSearchDialog::isTitleSearched() const
{
    return b_isTitleSearched;
}

// *****************************************************************
// private override
// *****************************************************************

void
TP_FileSearchDialog::accept()
{
    if( ui->lineEdit->text().isEmpty() )
    {
        QMessageBox::warning(
                    this,
                    tr( "Warning" ),
                    tr( "Please enter the keyword." )
                    );
    }
    else if( ! ui->checkBox_Filename->isChecked() &&
             ! ui->checkBox_Album->isChecked() &&
             ! ui->checkBox_Artist->isChecked() &&
             ! ui->checkBox_Title->isChecked() )
    {
        QMessageBox::warning(
                    this,
                    tr( "Warning" ),
                    tr( "Please select at least 1 search option." )
                    );
    }
    else
    {
        qstr_keyword = ui->lineEdit->text();
        b_isFilenameSearched = ui->checkBox_Filename->isChecked();
        b_isAlbumSearched = ui->checkBox_Album->isChecked();
        b_isArtistSearched = ui->checkBox_Artist->isChecked();
        b_isTitleSearched = ui->checkBox_Title->isChecked();

        QDialog::accept();
    }
}
