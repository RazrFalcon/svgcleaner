#include <QtCore/QFileInfo>
#include <QtCore/QTime>

#include "someutils.h"

SomeUtils::SomeUtils(QObject *parent) :
    QObject(parent)
{
}

QString SomeUtils::prepareSize(const float bytes)
{
    float size = bytes;
    int i = 0;
    while (size > 1024 && i < 2) {
        size = size/1024;
        i++;
    }
    QStringList list;
    list<<tr("B")<<tr("KiB")<<tr("MiB");

    return QString::number(size,'f',1)+list.at(i);
}

QString SomeUtils::prepareTime(const float ms)
{
    QTime dt = QTime::fromString("0", "z");
    dt = dt.addMSecs(ms);
    QString timeStr;
    timeStr = QString(tr("%1h %2m %3s %4ms")).arg(dt.toString("hh"))
                                             .arg(dt.toString("mm"))
                                             .arg(dt.toString("ss"))
                                             .arg(dt.toString("zzz"));
    return timeStr.remove(QRegExp("^(00. )*"));
}

QString SomeUtils::findFile(const QString &name, const QString &defaultFolder)
{
    if (QFile( "../SVGCleaner/"+name).exists()) // Qt Creator shadow build
        return "../SVGCleaner/"+name;
    else if (QFile(name).exists()) // next to exe. Usual build/Windows.
        return name;
    else if (QFile(defaultFolder+name).exists()) // custom path
        return defaultFolder+name;
    return name;
}
