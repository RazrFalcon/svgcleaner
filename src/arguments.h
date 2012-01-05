#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <QStringList>
#include <QString>
#include <QList>

struct ToThread
{
    QStringList inputFiles;
    QStringList outputFiles;
    QString level;
    QString args;
    QString compressWith;
};

struct SVGInfo
{
    enum {INPUT,OUTPUT};
    float compress;
    int time;
    QStringList paths;
    QList<int> sizes;
    int elemInitial;
    int elemFinal;
    int attrInitial;
    int attrFinal;
    bool crashed;
};

#endif // ARGUMENTS_H
