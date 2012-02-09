#include <QtGui/QApplication>
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

    QString locale = QLocale::system().name().remove(QRegExp("_.*"));
    qDebug()<<locale;
    QString path = SomeUtils::findFile("svgcleaner_"+locale+".qm",
                                       "/usr/share/svgcleaner/");
    qDebug()<<path;
    QTranslator *translator = new QTranslator;
    if (translator->load(path))
        qApp->installTranslator(translator);
    else
        qDebug()<<"Can't load the translation.";

    MainWindow w;
    w.show();

    return app.exec();
}
