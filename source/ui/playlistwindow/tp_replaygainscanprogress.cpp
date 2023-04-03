#include "tp_replaygainscanprogress.h"
#include "ui_tp_replaygainscanprogress.h"

#include "tp_globalconst.h"

#include "tp_runnable_replaygainscanner.h"

#include <QListWidgetItem>
#include <QThreadPool>
#include <QUrl>
#include <QtConcurrent>

TP_ReplayGainScanProgress::TP_ReplayGainScanProgress( QWidget *parent ) :
    QDialog         { parent }
  , ui              { new Ui::TP_ReplayGainScanProgress }
  , threadPool      { new QThreadPool { this } }
  , b_isFinished    {}
  , b_isAborted     {}
{
    ui->setupUi( this );

    threadPool->setMaxThreadCount( QThread::idealThreadCount() - 1 );

    ui->tableWidget->setColumnWidth( 1, 80 );
    ui->tableWidget->horizontalHeader()->setSectionResizeMode( 1, QHeaderView::Fixed );
    ui->tableWidget->horizontalHeader()->setSectionResizeMode( 0, QHeaderView::Stretch );
}


TP_ReplayGainScanProgress::~TP_ReplayGainScanProgress()
{
    delete ui;
}


void
TP_ReplayGainScanProgress::addFile( QListWidgetItem *I_item )
{
    auto idx { ui->tableWidget->rowCount() };
    ui->tableWidget->insertRow( idx );
    ui->tableWidget->setItem( idx, 0,
                              new QTableWidgetItem { I_item->data( TP::role_URL ).toUrl().toLocalFile() }
                              );
    ui->tableWidget->setItem( idx, 1,
                              new QTableWidgetItem { tr( "Pending..." ) }
                              );
}


void
TP_ReplayGainScanProgress::showEvent( QShowEvent *event )
{
    QDialog::showEvent( event );

    auto count { ui->tableWidget->rowCount() };
    for( int i {}; i < count; i++ )
    {
        auto runnable { new TP_Runnable_ReplayGainScanner { ui->tableWidget->item( i, 0 )->text(), i } };

        connect( runnable,  &TP_Runnable_ReplayGainScanner::signal_onStart,
                 this,      &TP_ReplayGainScanProgress::slot_onStart );
        connect( runnable,  &TP_Runnable_ReplayGainScanner::signal_onFinish,
                 this,      &TP_ReplayGainScanProgress::slot_onFinish );

        threadPool->start( runnable );
    }

    auto qFuture { QtConcurrent::run( &TP_ReplayGainScanProgress::daemon, this ) };
}


void
TP_ReplayGainScanProgress::reject()
{
    if( b_isFinished )
        QDialog::reject();
    else if( ! b_isAborted )
        b_isAborted = true;
}

// *****************************************************************
// private slots
// *****************************************************************

void
TP_ReplayGainScanProgress::slot_onStart( int I_index )
{
    ui->tableWidget->item( I_index, 1 )
            ->setText( tr( "Running..." ) );
}


void
TP_ReplayGainScanProgress::slot_onFinish( int I_index, bool I_isSuccessful )
{
    if( I_isSuccessful )
        ui->tableWidget->item( I_index, 1 )
                ->setText( tr( "Finished" ) );
    else
    {
        ui->tableWidget->item( I_index, 1 )
                ->setText( tr( "Error" ) );
        ui->tableWidget->item( I_index, 0 )
                ->setBackground( QBrush { Qt::yellow } );
        ui->tableWidget->item( I_index, 1 )
                ->setBackground( QBrush { Qt::yellow } );
    }
}


void
TP_ReplayGainScanProgress::on_pushButton_clicked()
{
    reject();
}

// *****************************************************************
// private
// *****************************************************************

void
TP_ReplayGainScanProgress::daemon()
{
    while( threadPool->activeThreadCount() )
        if( b_isAborted )
        {
            ui->pushButton->setText( tr( "Aborting..." ) );
            ui->pushButton->setEnabled( false );
            threadPool->clear();
            threadPool->waitForDone();
        }

    b_isFinished = true;
    ui->pushButton->setText( tr( "&Close" ) );
    ui->pushButton->setEnabled( true );
}
