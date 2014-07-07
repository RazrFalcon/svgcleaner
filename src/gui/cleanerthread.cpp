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
#include <QtCore/QBuffer>
#include <QtCore/QTime>

#include "someutils.h"
#include "cleanerthread.h"

CleanerThread::CleanerThread(const ThreadData &data, QObject *parent)
    : QThread(parent),
      m_zipPath(QApplication::applicationDirPath() + "/7za"),
      m_cliPath(QApplication::applicationDirPath() + "/svgcleaner-cli")
{
    m_data = data;
    m_isForceStop = false;
#ifdef USE_IPC
    m_isAcquiring = false;
#endif
}

#ifdef USE_IPC
void CleanerThread::initCleaner()
{
    // create shared memory
    m_sharedMemory = QSharedPointer<QSharedMemory>(
                        new QSharedMemory("SvgCleanerMem_" + m_data.id, this));
    // TODO: calculate max size
    m_sharedMemory->create(5000);
    m_sharedMemory->attach();

    // create semaphores
    m_semaphore1 = QSharedPointer<SystemSemaphore>(
                       new SystemSemaphore("SvgCleanerSemaphore1_" + m_data.id,
                                           0, SystemSemaphore::Create));
    m_semaphore2 = QSharedPointer<SystemSemaphore>(
                       new SystemSemaphore("SvgCleanerSemaphore2_" + m_data.id,
                                           0, SystemSemaphore::Create));

    // create cleaner process
    startProcess();
}

void CleanerThread::startProcess()
{
    m_proc = QSharedPointer<QProcess>(new QProcess(this));

    QString cliPath = QApplication::applicationDirPath() + "/svgcleaner-cli";
    QStringList args;
    args.reserve(m_data.args.size() + 2);
    args << "--slave"; // enable communication mode via shared memory
    args << m_data.id; // child id, used while generating unique names for memory and semaphores
    args << m_data.args; // usual 'cli' arguments
    args << "--short-output";
    m_proc->start(cliPath, args);
    m_proc->waitForStarted();

    // wait for 'cli' init finished
    m_semaphore2->acquire();
}

void CleanerThread::stopProcess()
{
    writeToMemory("", "");
    // signal to 'cli' that memory contains paths
    m_semaphore1->release();
    // wait for file cleaned
    m_semaphore2->acquire();

    m_proc->waitForFinished();
    m_proc->close();
}

void CleanerThread::writeToMemory(const QString &inFile, const QString &outFile)
{
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    QDataStream out(&buffer);
    out << inFile << outFile;
    int size = buffer.size();
    char *to = (char*)m_sharedMemory->data();
    const char *from = buffer.data().data();
    memcpy(to, from, qMin(m_sharedMemory->size(), size));
}

QString CleanerThread::readMemory()
{
    QBuffer buffer;
    QDataStream in(&buffer);
    QString outData;
    buffer.setData((char*)m_sharedMemory->constData(), m_sharedMemory->size());
    buffer.open(QBuffer::ReadWrite);
    in >> outData;
    return outData;
}
#endif

void CleanerThread::invokeStartNext(const StringPair &pair)
{
    QMetaObject::invokeMethod(this, "startNext", Qt::QueuedConnection,
                              Q_ARG(QString, pair.first),
                              Q_ARG(QString, pair.second));
}

void CleanerThread::startNext(const QString &inFile, const QString &outFile)
{
    // first start check
#ifdef USE_IPC
    if (m_sharedMemory.isNull())
        initCleaner();
    if (m_proc.isNull())
        startProcess();
#endif

    SVGInfo info;
    info.inSize = QFile(inFile).size();
    info.inPath = inFile;
    info.outPath = outFile;

    QString tmpInFile = inFile;
    if (inFile.endsWith("z")) {
        tmpInFile.chop(1); // remove 'z'
        unzip(inFile, tmpInFile);
    }

    if (!QDir(QFileInfo(outFile).absolutePath()).exists())
        QDir().mkpath(QFileInfo(outFile).absolutePath());

#ifdef USE_IPC
    // write paths to shared memory
    writeToMemory(tmpInFile, outFile);

    QElapsedTimer cleaningTime;
    cleaningTime.start();

    // signal to 'cli' that memory contains paths
    m_semaphore1->release();

    // wait for file cleaned
    m_isAcquiring = true;
    bool flag = m_semaphore2->acquire();
    m_isAcquiring = false;

    if (m_isForceStop)
        return;

    // read memory filled by 'cli'
    QString outData = readMemory();
    // if memory still contains input data - than 'cli' is probably crashed
    if (outData == tmpInFile)
        outData.clear();

    // usualy means that 'cli' process is segfaulted or acquire timeout
    if (!flag || outData.isEmpty()) {
        m_proc->kill();
        m_proc->waitForFinished();
        info.errString = m_proc->readAllStandardError();
        if (info.errString.isEmpty()) {
            if (m_semaphore2->isTimeout())
                info.errString = "Timeout error";
        } else {
            info.errString = info.errString.split("\n").filter(QRegExp("^Error")).join("");
        }
        if (info.errString.isEmpty())
            info.errString = "Unknown error";
        m_proc.clear();
    } else {
        info.time = cleaningTime.nsecsElapsed();
        parseDefaultOutput(info, outData);
    }
#else
    QStringList args;
    args.reserve(m_data.args.size() + 3);
    args << tmpInFile << outFile << m_data.args << "--short-output";
    QProcess proc;
    QElapsedTimer cleaningTime;
    cleaningTime.start();
    proc.start(m_cliPath, args);
    proc.waitForFinished(60000); // 60 sec
    info.time = cleaningTime.elapsed();
    QString outData = proc.readAllStandardError();
    info.time = cleaningTime.nsecsElapsed();
    parseDefaultOutput(info, outData);
#endif

    if (    m_data.compressType == ThreadData::CompressAll
        || (m_data.compressType == ThreadData::CompressAsOrig && info.inPath.endsWith('z')))
    {
        zip(info.outPath);
        info.outPath = info.outPath + "z";
    }
    info.outSize = QFile(info.outPath).size();

    info.compress = ((qreal)info.outSize / info.inSize) * 100;
    if (info.compress > 100)
        info.compress = 100;
    else if (info.compress < 0)
        info.compress = 0;

    emit cleaned(info);
}

void CleanerThread::parseDefaultOutput(SVGInfo &info, const QString &outData)
{
    const QChar *str = outData.constData();
    const QChar *end = str + outData.size();
    QVector<int> vec;
    vec.reserve(4);
    while (str != end) {
        QString tmpStr;
        while (str->isPrint()) {
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
        if (outData.contains("Error:")) {
            QStringList list = outData.split("\n").filter("Error:");
            if (!list.isEmpty())
                info.errString = list.first();
        }
        if (info.errString.isEmpty())
            info.errString = "Unknown error";
    }
}

void CleanerThread::exit()
{
#ifdef USE_IPC
    if (!m_proc.isNull()) {
        stopProcess();
    }
    emit finished();
#endif
}

void CleanerThread::forceStop()
{
#ifdef USE_IPC
    if (!m_proc.isNull()) {
        m_isForceStop = true;
        if (m_isAcquiring)
            m_semaphore2->release();
        m_proc->kill();
    }
#endif
}

void CleanerThread::unzip(const QString &inFile, const QString &outFile)
{
    QProcess proc;
    QStringList args;
    args << "e" << "-so" << inFile;
    proc.start(m_zipPath, args);
    proc.waitForFinished();
    QFile file(outFile);
    if (file.open(QFile::WriteOnly)) {
        QTextStream out(&file);
        out << proc.readAll();
    }
}

void CleanerThread::zip(const QString &outFile)
{
    QFile(outFile + "z").remove();
    QProcess proc;
    QStringList args;
    args << "a" << "-tgzip" << "-y" << "-mx" + m_data.compressLevel << outFile + "z"
         << outFile;
    proc.start(m_zipPath, args);
    proc.waitForFinished();
    QFile(outFile).remove();
}
