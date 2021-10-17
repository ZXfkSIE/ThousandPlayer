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
{
    setMinimumDuration( 0 );
    setAutoClose( false );
    setModal( true );

    setWindowFlags( windowFlags() | Qt::FramelessWindowHint );
    setStyleSheet(
"TP_ProgressDialog"
"{"
"   background-color:"
"       qlineargradient("
"           spread:pad, x1:0, y1:0, x2:1, y2:0,"
"           stop:0 rgb(20, 20, 20),"
"           stop:1 rgb(200, 200, 200)"
"       );"
"}"
"QLabel"
"{"
"   color: white;"
"   background-color: rgba(0, 0, 0, 0);"
"}"
"QPushButton"
"{"
"   color: white;"
"}"
);

    setFixedSize( 480, height() );

    show();
    cancel();
}
