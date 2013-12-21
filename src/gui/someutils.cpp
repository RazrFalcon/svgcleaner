/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2013 Evgeniy Reizner
** Copyright (C) 2012 Andrey Bayrak, Evgeniy Reizner
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

#include <QtCore/QFileInfo>
#include <QtGui/QApplication>
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
    list << tr("B") << tr("KiB") << tr("MiB");

    return QString::number(size, 'f', 1)+list.at(i);
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

QString SomeUtils::findBinFile(const QString &name)
{
    QStringList names;
    names << name << name + ".exe";
    foreach (const QString &name, names) {
        // next to GUI
        if (QFile("./" + name).exists())
            return "./" + name;
        // MacOS package
        if (QFile(QApplication::applicationDirPath() + "/" + name).exists())
            return QApplication::applicationDirPath() + "/" + name;
        // Linux default install
        if (QFile("/usr/bin/" + name).exists())
            return "/usr/bin/" + name;
    }
    return "";
}

QString SomeUtils::genSearchFolderList()
{
    QString paths;
    paths += QApplication::applicationDirPath() + "/\n";
#ifdef Q_OS_LINUX
    paths += "/usr/bin/\n";
#endif
    return paths;
}
