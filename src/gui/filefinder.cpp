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
