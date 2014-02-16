/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2012-2014 Evgeniy Reizner
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**
****************************************************************************/

#include <QtCore/QTime>
#include <QtCore/QStringList>

#include "someutils.h"

QString SomeUtils::prepareSize(const quint32 bytes)
{
    qreal size = bytes;
    int i = 0;
    while (size > 1024 && i < 2) {
        size = size / 1024.0;
        i++;
    }
    static QStringList list = QStringList() << QObject::tr("B") << QObject::tr("KiB")
                                            << QObject::tr("MiB");
    return QString::number(size, 'f', 1) + " " + list.at(i);
}

QString SomeUtils::prepareTime(const quint64 ms)
{
    QTime t(0, 0);
    t = t.addMSecs(ms);
    QString timeStr;
    if (t.hour() != 0)
        timeStr += t.toString("hh") + QObject::tr("h");
    if (!timeStr.isEmpty() || t.minute() != 0)
        timeStr += " " + t.toString("mm") + QObject::tr("m");
    if (!timeStr.isEmpty() || t.second() != 0)
        timeStr += " " + t.toString("ss") + QObject::tr("s");
    timeStr += " " + t.toString("zzz") + QObject::tr("ms");
    return timeStr;
}
