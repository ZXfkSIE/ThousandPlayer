#include "tp_progressdialog.h"

#include <QLabel>

TP_ProgressDialog::TP_ProgressDialog(
        const QString &labelText,
        const QString &cancelButtonText,
        int minimum,
        int maximum,
        QWidget *parent,
        Qt::WindowFlags f
        ) :
    QProgressDialog { labelText, cancelButtonText, minimum, maximum, parent, f }
  , nextPercentage { percentageStep }
  , n_finished {}
  , idx_min {}
  , idx_max {}
{
    setCancelButton( nullptr );
    setMinimumDuration( 0 );
    setModal( true );

    setWindowFlags( windowFlags() | Qt::FramelessWindowHint );
    setStyleSheet(
"TP_ProgressDialog"
"{"
"   background-color:"
"       qlineargradient("
"           spread:pad, x1:0, y1:0, x2:1, y2:0,"
"           stop:0   rgb( 20, 20, 20 ),"
"           stop:0.6 rgb( 80, 80, 80 ),"
"           stop:1   rgb( 230, 230, 230 )"
"       );"
"}"
"QLabel { color: white; background-color: rgba( 0, 0, 0, 0 ); }"
"QProgressBar { color: white; }"
);

    setFixedSize( 480, height() );
}


void
TP_ProgressDialog::initialize( int I_max, int I_idx_min, int I_idx_max )
{
    reset();
    setMaximum( I_max );
    nextPercentage = percentageStep;
    n_finished = 0;
    idx_min = I_idx_min;
    idx_max = I_idx_max;
}

// *****************************************************************
// public slots:
// *****************************************************************

void
TP_ProgressDialog::slot_addCount()
{
    n_finished++;
    auto currentPercentage { n_finished * 100 / maximum() };

    if( n_finished == maximum() )
    {
        setValue( n_finished );
        emit signal_onComplete( idx_min, idx_max );
    }
    else if( currentPercentage >= nextPercentage )
    {
        while( currentPercentage >= nextPercentage )
            nextPercentage += percentageStep;

        setValue( n_finished );
    }
}
