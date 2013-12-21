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
#include <QtCore/QFileInfo>
#include <QtCore/QtDebug>
#include <QtCore/QTime>

#include "cleanerthread.h"

CleanerThread::CleanerThread(ToThread args, QObject *parent) :
    QObject(parent)
{
    arguments = args;
    proc = new QProcess(this);
    proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(proc, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(proc, SIGNAL(finished(int)), this, SLOT(finished(int)));
}

void CleanerThread::startNext(const QString &inFile, const QString &outFile)
{
    cleaningTime.start();
    scriptOutput.clear();
    outSVG = QString(outFile).replace("svgz", "svg").replace("SVGZ", "SVG");
    currentIn = inFile;
    currentOut = outFile;

    QDir().mkpath(QFileInfo(outFile).absolutePath());
    if (QFileInfo(inFile).suffix().toLower() == "svg") {
        if (inFile != outFile)
            QFile(outSVG).remove();
        QFile().copy(inFile, outSVG);
    } else {
        unzip(inFile);
    }

    QStringList args;
    args.append(outSVG);
    args.append(outSVG);
    args.append(arguments.args);
    proc->start(arguments.cliPath, args);
    proc->waitForFinished();
}

void CleanerThread::unzip(const QString &inPath)
{
    QProcess proc;
    QStringList args;
    args << "e" << "-so" << inPath;
    proc.start(arguments.zipPath, args);
    proc.waitForFinished();
    QFile file(outSVG);
    if (file.open(QFile::WriteOnly)) {
        QTextStream out(&file);
        out << proc.readAll();
        file.close();
    }
}

void CleanerThread::readyRead()
{
    scriptOutput += proc->readAll();
}

void CleanerThread::finished(int)
{
    if (QFileInfo(currentOut).suffix() == "svgz") {
        if (currentOut == currentIn) // because 7zip can't overwrite the svgz, for some reason
            QFile(currentIn).remove();
        QFile(currentOut).remove(); // if it's already exist

        QProcess procZip;
        QStringList args;
        args << "a" << "-tgzip" << "-mx" + arguments.compressLevel << currentOut << outSVG;
        procZip.start(arguments.zipPath, args);
        procZip.waitForFinished();
        QFile(outSVG).remove();
    }
    emit cleaned(info());
}

// generate files info
SVGInfo CleanerThread::info()
{
    SVGInfo info;
    info.inPath  = currentIn;
    info.outPath = currentOut;

    int initialFileSize = 0;
    foreach (const QString &text, scriptOutput.split("\n")) {
        if      (text.contains("The initial number of elements is:"))
            info.elemInitial = QString(text).remove(QRegExp(".*: |\"")).toInt();
        else if (text.contains("The final number of elements is:"))
            info.elemFinal   = QString(text).remove(QRegExp(".*: |\"")).toInt();
        else if (text.contains("The initial number of attributes is:"))
            info.attrInitial = QString(text).remove(QRegExp(".*: |\"")).toInt();
        else if (text.contains("The final number of attributes is:"))
            info.attrFinal   = QString(text).remove(QRegExp(".*: |\"")).toInt();
        else if (text.contains("The initial file size is:"))
            initialFileSize  = QString(text).remove(QRegExp(".*: |\"")).toInt();
    }

    // if svgz saved to svg with cleaning, we need to save size of uncompressed/uncleaned svg
    if ((QFileInfo(currentOut).suffix()  == "svg"
        && QFileInfo(currentIn).suffix() == "svgz")
        || (currentIn == currentOut)) {

        info.inSize = initialFileSize;
        info.compress = ((float)QFile(currentOut).size() / initialFileSize) * 100;
    } else {
        info.inSize = QFile(currentIn).size();
        info.compress = ((float)QFile(currentOut).size()
                               / QFile(currentIn).size()) * 100;
    }
    if (info.compress > 100)
        info.compress = 100;
    else if (info.compress < 0)
        info.compress = 0;

    info.outSize = QFileInfo(currentOut).size();

    info.time = cleaningTime.elapsed();
    if (scriptOutput.contains("ASSERT:"))
        info.errString = tr("Crashed");
    else if (scriptOutput.contains("Error")) {
        if (scriptOutput.contains("Error: input file does not exist."))
            info.errString = tr("Input file does not exist.");
        else if (scriptOutput.contains("Error: output folder does not exist."))
            info.errString = tr("Output folder does not exist.");
        else if (scriptOutput.contains("Error: it's a not well-formed SVG file."))
            info.errString = tr("It's a not well-formed SVG file.");
        else
            info.errString = tr("Crashed");
    }

    return info;
}

// find values in svgcleaner output
int CleanerThread::findValue(const QString &text)
{
    if (!scriptOutput.contains(text))
        return 0;
    QString tmpStr = scriptOutput.split("\n").filter(text).first();
    return tmpStr.remove(QRegExp(".*: |\"")).toInt();
}
