#include <QApplication>
#include <QDebug>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    const QStringList args = a.arguments();
    if (args.size() != 7) {
        qDebug() << "Usage:\n  err_view icons/file.svg original.png cleaned.png diff.png 59 config.json";
        return 0;
    }

    MainWindow w;
    w.setSvgFile(args.at(1));
    w.setFiles(args.at(2), args.at(3), args.at(4));
    w.setAE(args.at(5));
    w.setJsonPath(args.at(6));
    w.adjustSize();
    w.show();

    return a.exec();
}
