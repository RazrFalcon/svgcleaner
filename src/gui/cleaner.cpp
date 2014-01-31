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

#include <QtGui/QApplication>
#include <QtCore/QDir>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QtDebug>

#include "someutils.h"
#include "cleaner.h"

SVGInfo Cleaner::cleanFile(const ToThread &data)
{
    if (!QDir(QFileInfo(data.outputFile).absolutePath()).exists())
        QDir().mkpath(QFileInfo(data.outputFile).absolutePath());
    SVGInfo info;
    info.inSize = QFile(data.inputFile).size();

    QString inFile = data.inputFile;
    if (data.decompress) {
        unzip(data);
        inFile = data.outputFile;
    }

    QStringList args;
    args.reserve(data.args.size() + 3);
    args << inFile << data.outputFile  << data.args << "--short-output";
    QProcess proc;
    QElapsedTimer cleaningTime;
    cleaningTime.start();
    static QString cliPath = QApplication::applicationDirPath() + "/svgcleaner-cli";
    proc.start(cliPath, args);
    proc.waitForFinished(300000); // 5min
    info.time = cleaningTime.elapsed();
    QString output = proc.readAllStandardError();

    info.inPath  = data.inputFile;

    const QChar *str = output.constData();
    const QChar *end = str + output.size();
    QVector<int> vec;
    vec.reserve(4);
    while (str != end) {
        QString tmpStr;
        while (*str != '\n') {
            tmpStr += *str;
            ++str;
        }
        bool ok = false;
        int num = tmpStr.toInt(&ok);
        if (ok)
            vec << num;
        ++str;
    }
    if (vec.size() == 4) {
        info.elemInitial = vec.at(0);
        info.attrInitial = vec.at(1);
        info.elemFinal   = vec.at(2);
        info.attrFinal   = vec.at(3);
    } else {
        if (output.contains("Error:"))
            info.errString = output.split("\n").filter("Error:").first();
        else
            info.errString = "Unknown error";
    }

    if (data.compress) {
        zip(data);
        info.outSize = QFile(data.outputFile + "z").size();
        info.outPath = data.outputFile + "z";
    } else {
        info.outSize = QFile(data.outputFile).size();
        info.outPath = data.outputFile;
    }

    info.compress = ((qreal)info.outSize / info.inSize) * 100;
    if (info.compress > 100)
        info.compress = 100;
    else if (info.compress < 0)
        info.compress = 0;

    return info;
}

void Cleaner::unzip(const ToThread &data)
{
    QProcess proc;
    QStringList args;
    args << "e" << "-so" << data.inputFile;
    static QString zipPath = QApplication::applicationDirPath() + "/7za";
    proc.start(zipPath, args);
    proc.waitForFinished();
    QFile file(data.outputFile);
    if (file.open(QFile::WriteOnly)) {
        QTextStream out(&file);
        out << proc.readAll();
    }
}

void Cleaner::zip(const ToThread &data)
{
    QFile(data.outputFile + "z").remove();
    QProcess proc;
    QStringList args;
    args << "a" << "-tgzip" << "-y" << "-mx" + data.compressLevel << data.outputFile + "z"
         << data.outputFile;
    static QString zipPath = QApplication::applicationDirPath() + "/7za";
    proc.start(zipPath, args);
    proc.waitForFinished();
    QFile(data.outputFile).remove();
}
