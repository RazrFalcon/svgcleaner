#include <QtCore/QLibraryInfo>
#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>
#include <QtCore/QDir>
#include <QtGui/QApplication>

#include <QtDebug>

#include "someutils.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationVersion("0.5");

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(  QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(      QTextCodec::codecForName("UTF-8"));

    // load translation for SVG Cleaner
    QString locale = QLocale::system().name();
#ifdef Q_OS_LINUX
    app.addLibraryPath("/usr/share/svgcleaner/translations");
#endif

    QTranslator translator;
#ifdef Q_OS_MAC
    if (translator.load("svgcleaner_" + locale, "../translations"))
        app.installTranslator(&translator);
#else
    if (translator.load("svgcleaner_" + locale, "translations"))
        app.installTranslator(&translator);
#endif

    // load translation for Qt
    QTranslator qtTranslator;
#ifdef Q_OS_WIN
    if (qtTranslator.load("qt_" + locale, "translations"))
        app.installTranslator(&qtTranslator);
#elif defined(Q_OS_MAC)
    if (qtTranslator.load("qt_" + locale, "../translations"))
        app.installTranslator(&qtTranslator);
#else
    if (qtTranslator.load("qt_" + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        app.installTranslator(&qtTranslator);
#endif

    MainWindow w;
    w.show();

    return app.exec();
}
