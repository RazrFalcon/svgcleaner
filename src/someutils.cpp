#include <QTime>
#include <QFileInfo>
#include <QtDebug>

#include "someutils.h"

SomeUtils::SomeUtils(QObject *parent) :
    QObject(parent)
{
}

QString SomeUtils::prepareSize(int bytes)
{
    double size = bytes;
    int i = 0;
    while (size > 1024 && i < 4) {
        size = size/1024;
        i++;
    }
    QStringList list;
    list<<tr("B")<<tr("KiB")<<tr("MiB")<<tr("GiB");

    return QString::number(size,'f',1)+list.at(i);
}

QString SomeUtils::prepareTime(int ms, bool clip)
{
    QTime dt = QTime::fromString("0", "z");
    dt = dt.addMSecs(ms);
    QString timeStr;
//    if (clip) {
        timeStr = QString("%1h %2m %3s %4ms").arg(dt.toString("hh"))
                                          .arg(dt.toString("mm"))
                                          .arg(dt.toString("ss"))
                                          .arg(dt.toString("zzz"));
        return timeStr.remove(QRegExp("00. "));
//    } else {
//        return dt.toString("hh:mm:ss:zzz");
//        QString status = QString("Processing file %1 of %2: %3")
//                         .arg(i).arg(total).arg(fileName);
//        timeStr = QString("%1h %2m %3s %4ms").arg(dt.toString("hh"))
//                                          .arg(dt.toString("mm"))
//                                          .arg(dt.toString("ss"))
//                                          .arg(dt.toString("zzz"));
//        return timeStr;
//    }
    return "00:00:00:000";
}

int SomeUtils::getTotalSize(const QStringList &list, int count)
{
    int size = 0;
    if (count == -1)
        count = list.count();
    for (int i = 0; i < count; ++i)
        size += QFileInfo(list.at(i)).size();
    return size;
}
