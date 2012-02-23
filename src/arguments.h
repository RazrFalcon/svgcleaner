#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <QStringList>
#include <QString>
#include <QList>

struct ToThread
{
    QStringList inputFiles;
    QStringList outputFiles;
    QStringList args;
    QString level;
    QString cleanerPath;
    QString zipPath;
    QString perlPath;
};

struct SVGInfo
{
    float compress;
    int attrFinal;
    int attrInitial;
    int elemFinal;
    int elemInitial;
    int inSize;
    int outSize;
    int time;
    QString errString;
    QString inPath;
    QString outPath;
};

#endif // ARGUMENTS_H
