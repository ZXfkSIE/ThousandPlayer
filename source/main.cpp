#include "tp_mainclass.h"

#include <QApplication>
#include <QLocale>
#include <QTimer>
#include <QTranslator>

#include "tp_coverviewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages)
    {
        const QString baseName = "ThousandPlayer_" + QLocale(locale).name();
        if ( translator.load( ":/i18n/" + baseName ) )
        {
            a.installTranslator( &translator );
            break;
        }
    }

    TP_MainClass mainClass {};

    QTimer::singleShot( 150, &mainClass, &TP_MainClass::slot_initializePosition );
    mainClass.slot_initializePosition();

    // Need to be executed after QApplication object executed its exec()
    QTimer::singleShot( 300, &mainClass, &TP_MainClass::slot_checkIfServiceAvailable );
    return a.exec();
}
