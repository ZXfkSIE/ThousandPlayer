#include "tp_filesearchdialog.h"
#include "ui_tp_filesearchdialog.h"

TP_FileSearchDialog::TP_FileSearchDialog( QWidget *parent ) :
    QDialog { parent }
  , ui      { new Ui::TP_FileSearchDialog }
{
    ui->setupUi( this );
}

TP_FileSearchDialog::~TP_FileSearchDialog()
{
    delete ui;
}
