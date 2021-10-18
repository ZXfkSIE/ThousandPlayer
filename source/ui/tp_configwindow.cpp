#include "tp_configwindow.h"
#include "ui_tp_configwindow.h"

TP_ConfigWindow::TP_ConfigWindow( QWidget *parent ) :
    QDialog { parent }
  , ui      { new Ui::TP_ConfigWindow }
{
    ui->setupUi( this );
}

TP_ConfigWindow::~TP_ConfigWindow()
{
    delete ui;
}
