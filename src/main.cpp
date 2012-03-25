#include <QtGui/QApplication>
#include <QLibraryInfo>
#include <QTextCodec>
#include <QTranslator>
#include <QtDebug>

#include "someutils.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    // FIXME: rewrite to libraryPath()
    // load translation for SVG Cleaner
    QString locale = QLocale::system().name();
#ifdef Q_OS_WIN
    QString path = SomeUtils::findFile(QString("svgcleaner_%1.qm").arg(locale),
                                       "./translations/");
#else
    QString path = SomeUtils::findFile(QString("svgcleaner_%1.qm").arg(locale),
                                       "/usr/share/svgcleaner/translations/");
#endif
    QTranslator translator;
    if (translator.load(path)) {
        app.installTranslator(&translator);
        qDebug()<<"translator path:"<<path;
    }

    // load translation for Qt
    QTranslator qtTranslator;
    qtTranslator.load("qt_"+locale,QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    MainWindow w;
    w.show();

    return app.exec();
}
