#include <QDir>
#include <QFileInfo>
#include <QTime>
#include <QtDebug>

#include "cleanerthread.h"

CleanerThread::CleanerThread(ToThread args, QObject *parent) :
    QObject(parent)
{
    arguments = args;
    proc = new QProcess;
    connect(proc,SIGNAL(readyReadStandardOutput()),this,SLOT(readyRead()));
    connect(proc,SIGNAL(readyReadStandardError()),this,SLOT(readyReadError()));
    connect(proc,SIGNAL(finished(int)),this,SLOT(finished(int)));
}

void CleanerThread::startNext(const QString &inFile, const QString &outFile)
{
    cleaningTime = QTime::currentTime();

    scriptOutput.clear();
    scriptErrors.clear();
    outSVG = QString(outFile).replace("svgz","svg");
    currentIn = inFile;
    currentOut = outFile;

    QDir().mkpath(QFileInfo(outFile).absolutePath());
    if (QFileInfo(inFile).suffix() == "svg") {
        if (inFile != outFile)
            QFile(outSVG).remove();
        QFile().copy(inFile,outSVG);
    }
    else
        unzip(inFile);

    QStringList args;
    args.append(arguments.cleanerPath);
    args.append("--in-file="+outSVG);
    args.append("--out-file="+outSVG);
    args.append(arguments.args);
    proc->start(arguments.perlPath,args);
}

void CleanerThread::unzip(const QString &inPath)
{
    QProcess proc;
    QStringList args;
    args<<"e"<<"-so"<<inPath;
    proc.start(arguments.zipPath,args);
    proc.waitForFinished();
    QFile file(outSVG);
    if (file.open(QFile::WriteOnly)) {
        QTextStream out(&file);
        out<<proc.readAll();
        file.close();
    }
}

void CleanerThread::readyRead()
{
    scriptOutput += proc->readAllStandardOutput();
}

void CleanerThread::readyReadError()
{
    QString error = proc->readAllStandardError();
    if (error.contains("Can't locate XML/Twig.pm in"))
        emit criticalError(tr("You have to install XML::Twig module."));
    else
        qDebug()<<error<<"in file"<<currentIn;
    scriptErrors += error;
}

void CleanerThread::finished(int)
{
    if (QFileInfo(currentOut).suffix() == "svgz") {
        if (currentOut == currentIn) // because 7zip can't overwrite the svgz, for some reason
            QFile(currentIn).remove();
        QFile(currentOut).remove(); // if it's already exist

        QProcess procZip;
        QStringList args;
        args<<"a"<<"-tgzip"<<"-mx"+arguments.level<<currentOut<<outSVG;
        procZip.start(arguments.zipPath,args);
        procZip.waitForFinished();
        QFile(outSVG).remove();
    }
    emit cleaned(info());
}

// generate files info
SVGInfo CleanerThread::info()
{
    SVGInfo info;
    info.inPath = currentIn;
    info.outPath = currentOut;
    info.elemInitial = findValue("The initial number of elements is");
    info.elemFinal = findValue("The final number of elements is");
    info.attrInitial = findValue("The initial number of attributes is");
    info.attrFinal = findValue("The final number of attributes is");

    // if svgz saved to svg with cleaning, we need to save size of uncompressed/uncleaned svg
    if ((QFileInfo(currentOut).suffix() == "svg"
        && QFileInfo(currentIn).suffix() == "svgz")
        || (currentIn == currentOut)) {

        info.inSize = findValue("The initial file size is");
        info.compress = ((float)QFileInfo(currentOut).size()
                               /findValue("The initial file size is"))*100;
    } else {
        info.inSize = QFileInfo(currentIn).size();
        info.compress = ((float)QFileInfo(currentOut).size()
                               /QFileInfo(currentIn).size())*100;
    }
    if (info.compress > 100)
        info.compress = 100;
    else if (info.compress < 0)
        info.compress = 0;

    info.outSize = QFileInfo(currentOut).size();

    info.time = cleaningTime.elapsed();

    info.errString = "";
    if (scriptOutput.isEmpty())
        info.errString = "Crashed";
    if (scriptErrors.contains("It's a not well-formed SVG file!"))
        info.errString = tr("It's a not well-formed\nSVG file!");
    if (scriptErrors.contains("This file doesn't need cleaning!"))
        info.errString = tr("This file doesn't\nneed cleaning!");

    if (arguments.args.contains("--quiet=no"))
        qDebug()<<scriptOutput;

    return info;
}

// find values in svgcleaner output
int CleanerThread::findValue(const QString &text)
{
    if (!scriptOutput.contains(text))
        return 0;
    QString tmpStr = scriptOutput.split("\n").filter(text).first();
    return tmpStr.remove(QRegExp("[A-Za-z]|%| ")).toInt();
}
