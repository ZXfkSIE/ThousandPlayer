#include "tp_runnable_replaygainscanner.h"

#include "tp_globalfunction.h"
#ifdef Q_OS_WIN
#include "tp_globalvariable.h"
#endif

#include <QProcess>

TP_Runnable_ReplayGainScanner::TP_Runnable_ReplayGainScanner( const QString &I_qstr_path,
                                                              unsigned I_index ) :
    QObject     { nullptr }
  , qstr_path   { I_qstr_path }
  , index       { I_index }
{
}


void
TP_Runnable_ReplayGainScanner::run()
{
    emit signal_onStart( index );

    QProcess qProcess {};
    QStringList arguments { { "-s" }, { "i" } };
    if( TP::getAudioFormat( qstr_path ) == TP::AudioFormat::MP3 )
        arguments << "-I" << "4" << "-S";
    arguments << qstr_path;

    // qDebug() << "RG scan args:" << arguments;

    QString qstr_exe {};

#ifdef Q_OS_LINUX
    qstr_exe = QString { "loudgain" };
#endif
#ifdef Q_OS_WIN
    qstr_exe = TP::config().getRsgainPath();
#endif

    qProcess.start( qstr_exe, arguments );
    qProcess.waitForFinished();
    if( qProcess.exitStatus() != QProcess::NormalExit || qProcess.exitCode() )
    {
        qDebug() << "RGScan error:" << qProcess.errorString()
                 << ", stdout:" << QString { qProcess.readAllStandardOutput() }
                 << ", stderr:" << QString { qProcess.readAllStandardError() };
        emit signal_onFinish( index, false );
    }
    else
        emit signal_onFinish( index, true );
}
