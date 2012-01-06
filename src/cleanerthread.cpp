#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QtSvg/QSvgRenderer>
#include <QDomDocument>
#include <QTextStream>
#include <QTextDocument>
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

CleanerThread::~CleanerThread()
{
}

void CleanerThread::startNext(const QString &inFile, const QString &outFile)
{
    cleaningTime = QTime::currentTime();

    scriptOutput.clear();
    outSVG = QString(outFile).replace("svgz","svg");
    currentIn = inFile;
    currentOut = outFile;

    if (validXML(inFile)) {
        QString str = prepareFile(inFile);
        if (str.isEmpty()) { // = bad svg file
            emit cleaned(info());
            return;
        }
        QDir().mkpath(QFileInfo(outFile).absolutePath());
        QFile outFile(outSVG);
        if (outFile.open(QFile::WriteOnly)) {
            QTextStream out(&outFile);
            out<<str;
        }
    }
    QStringList args;
    args.append("/usr/bin/svgcleaner");
    args.append(outSVG);
    args.append(arguments.args);
    args.append("quiet");

    proc->start("perl",args);
}

// valid svg structure using Qt class QSvgRenderer
bool CleanerThread::validXML(const QString &file)
{
    QSvgRenderer render;
    render.load(file);
    return render.isValid();
}

// remove attribute xml:space before starting a script,
// in the same time copy svg source to the new path
QString CleanerThread::prepareFile(const QString &file)
{
    QDomDocument inputDom;
    if (QFileInfo(file).suffix() == "svg") {
        QFile inputFile(file);
        if (inputFile.open(QFile::ReadOnly))
            inputDom.setContent(&inputFile);
    } else {
        QProcess proc;
        QStringList args;
        args<<"-c"<<file;
        proc.start("gunzip",args);
        proc.waitForFinished();
        inputDom.setContent(&proc);
    }
    QDomNodeList nodeList = inputDom.childNodes();
    for (int i = 0; i < nodeList.count(); ++i) {
        if (nodeList.at(i).nodeName() == "svg" || nodeList.at(i).nodeName() == "svg:svg") {
            QDomElement element = nodeList.at(i).toElement();
            element.removeAttribute("xml:space");
            QRegExp rx("px|pt|pc|mm|cm|m|in|ft|em|ex|%");
            QString width = element.attribute("width");
            width.remove(rx);
            QString height = element.attribute("height");
            height.remove(rx);
            if (width.toInt() < 1 || height.toInt() < 1)
                return "";
            return inputDom.toString();
        }
    }
    return "";
}

void CleanerThread::readyRead()
{
    scriptOutput += proc->readAllStandardOutput();
}

void CleanerThread::readyReadError()
{
    QString error = proc->readAllStandardError();
    if (error.contains("Can't locate XML/Twig.pm in"))
        emit criticalError(tr("You must install XML/Twig."));
    else
        qDebug()<<error;
}

void CleanerThread::finished(int)
{
    if (QFileInfo(currentOut).suffix() == "svgz") {
        QProcess procZip;
        QStringList args;
        if (arguments.compressWith == "gzip") {
            args<<"--suffix=z"<<"-"+arguments.level<<"-f"<<outSVG;
        } else {
            args<<"a"<<"-bd"<<"-tgzip"<<"-mx"+arguments.level<<currentOut<<outSVG;
        }
        procZip.start(arguments.compressWith,args);
        procZip.waitForFinished();
        if (arguments.compressWith == "7z") // temp bugfix
            QFile(outSVG).remove();
    }
    emit cleaned(info());
}

// generate files info
SVGInfo CleanerThread::info()
{
    SVGInfo info;

    info.paths<<currentIn<<currentOut;
    info.elemInitial = findValue("The initial number of elements is");
    info.elemFinal = findValue("The final number of elements is");
    info.attrInitial = findValue("The initial number of attributes is");
    info.attrFinal = findValue("The final number of attributes is");

    // if svgz saved to svg with cleaning, we need to save size of uncompressed/uncleaned svg
    if (  QFileInfo(currentOut).suffix() == "svg"
        && QFileInfo(currentIn).suffix() == "svgz") {

        info.sizes<<findValue("The initial file size is");
        info.compress = ((float)QFileInfo(currentOut).size()
                              /findValue("The initial file size is"))*100;
    } else {
        info.sizes<<QFileInfo(currentIn).size();
        info.compress = ((float)QFileInfo(currentOut).size()
                              /QFileInfo(currentIn).size())*100;
    }
    if (info.compress > 100)
        info.compress = 100;
    else if (info.compress < 0)
        info.compress = 0;

    info.sizes<<QFileInfo(currentOut).size();

    info.time = cleaningTime.elapsed();
    info.crashed = scriptOutput.isEmpty(); // true - mean crash
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
