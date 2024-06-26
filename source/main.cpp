﻿#include "tp_mainclass.h"

#include <QApplication>
#include <QDirIterator>
#include <QLocale>
#include <QTimer>
#include <QTranslator>

int main( int argc, char *argv[] )
{
    qDebug() << "[Main Function] the files integrated by the executable file are:";
    QDirIterator it { ":", QDirIterator::Subdirectories };
    while ( it.hasNext() )
    {
        it.next();
        if( ! it.filePath().contains( QString { ":/q" }, Qt::CaseSensitive ) )
            qDebug() << it.filePath();
    }

    QApplication a { argc, argv };

    QTranslator translator;
    auto uiLanguages { QLocale::system().uiLanguages() };
    for ( const QString &locale : uiLanguages )
    {
        const QString baseName = "ThousandPlayer_" + QLocale { locale }.name();
        qDebug() << "[Main Function] installing i18n file" << baseName;
        if ( translator.load( ":/i18n/" + baseName ) )
        {
            a.installTranslator( &translator );
            qDebug() << "[Main Function] The i18n file" << baseName << "has beed installed.";
            break;
        }
    }

    TP_MainClass mainClass {};

    // Delay the position initialization to avoid some painting problem mainly in Linux
    QTimer::singleShot( 250, &mainClass, &TP_MainClass::slot_initializePosition );

    // Need to be executed after QApplication object executed its exec()
    QTimer::singleShot( 500, &mainClass, &TP_MainClass::slot_checkIfServiceAvailable );
    return a.exec();
}
