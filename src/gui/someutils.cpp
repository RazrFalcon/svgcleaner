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
    while (size > 1024.0 && i < 2) {
        size = size / 1024.0;
        i++;
    }
    static const QStringList list = QStringList()
        << QObject::tr("B") << QObject::tr("KiB") << QObject::tr("MiB");
    return QString::number(size, 'f', 1) + " " + list.at(i);
}

QString SomeUtils::prepareTime(const quint64 nsec)
{
    QTime t(0, 0);
    t = t.addMSecs(nsec/1000000);

    int tm[3];
    tm[0] = t.hour();
    tm[1] = t.minute();
    tm[2] = t.second();

    static const QStringList list = QStringList()
        << QObject::tr("h") << QObject::tr("m") << QObject::tr("s") << QObject::tr("ms");

    QString timeStr;
    for (int i = 0; i < 3; ++i) {
        if (!timeStr.isEmpty() || tm[i] != 0) {
            if (!timeStr.isEmpty())
                timeStr += " ";
            timeStr += QString::number(tm[i]).rightJustified(2, '0') + list.at(i);
        }
    }

    if (!timeStr.isEmpty())
        timeStr += " " + QString::number(t.msec()).rightJustified(3, '0');
    else {
        // if string contains only ms - add nsec
        timeStr += QString::number(t.msec());
        timeStr += "." + QString::number(nsec % 1000000).left(1);
    }
    timeStr += QObject::tr("ms");
    return timeStr;
}

