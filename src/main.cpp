#include <QtCore/QLibraryInfo>
#include <QtCore/QTextCodec>
#include <QtCore/QTranslator>
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
#ifdef Q_OS_WIN
    app.addLibraryPath(QDir::toNativeSeparators(QApplication::applicationDirPath() +
                                                "/translations"))
//    app.addLibraryPath("./translations");
#else
    app.addLibraryPath("/usr/share/svgcleaner/translations");
#endif
    qDebug()<<QApplication::libraryPaths();

    QTranslator translator;
    if (translator.load("svgcleaner_"+locale))
        app.installTranslator(&translator);

    // load translation for Qt
    QTranslator qtTranslator;
    qtTranslator.load("qt_"+locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    MainWindow w;
    w.show();

    return app.exec();
}
