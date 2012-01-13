#include <QtGui/QApplication>
#include <QTranslator>
#include <QTextCodec>
#include <QtDebug>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    QTranslator *translator = new QTranslator;
#ifdef Q_OS_WIN
    QString path = "translations/svgcleaner_";
#else
    QString path = "/usr/share/svgcleaner/translations/svgcleaner_";
#endif
    if (translator->load(path+QLocale::system().name().remove(QRegExp("_.."))))
        qApp->installTranslator(translator);

    MainWindow w;
    w.show();

    return a.exec();
}

