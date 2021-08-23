#include "tp_mainclass.h"

#include <QApplication>
#include <QLocale>
#include <QTimer>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ThousandPlayer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    TP_MainClass mainClass {};
    mainClass.slot_checkIfServiceAvailable();
    mainClass.slot_initializePosition();

    //QTimer::singleShot(100, &mainClass, &TP_MainClass::slot_checkIfServiceAvailable);
    //QTimer::singleShot(200, &mainClass, &TP_MainClass::slot_initializePosition);

    return a.exec();
}
