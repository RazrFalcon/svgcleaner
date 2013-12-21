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

#include <QtCore/QDir>

#include "filefinder.h"

FileFinder::FileFinder(QObject *parent) :
    QObject(parent)
{
}

void FileFinder::startSearch(const QString &startDir, bool recursive)
{
    stop = false;
    QFileInfoList list = searchForFiles(startDir, recursive);
    emit finished(list);
}

QFileInfoList FileFinder::searchForFiles(const QString &startDir, bool recursive)
{
    QDir dir(startDir);
    QFileInfoList list;
    foreach (QString file, dir.entryList(QStringList() << "*.svg" << "*.svgz",  QDir::Files))
        list += QFileInfo(startDir + "/" + file);
    if (recursive && !stop) {
        foreach (QString subdir, dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot))
            list += searchForFiles(startDir + "/" + subdir, recursive);
    }
    return list;
}

void FileFinder::stopSearch()
{
    stop = true;
}
