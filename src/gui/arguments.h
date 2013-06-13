#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QStringList>

struct ToThread
{
    QStringList inputFiles;
    QStringList outputFiles;
    QStringList args;
    QString compressLevel;
    QString cliPath;
    QString zipPath;
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
