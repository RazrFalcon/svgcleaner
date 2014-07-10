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

#ifndef CLEANERTHREAD_H
#define CLEANERTHREAD_H

#include <QtCore/QThread>
#include <QtCore/QProcess>
#include <QtDebug>

#ifdef USE_IPC
#include <QSharedMemory>
#include <QSharedPointer>
#include "../3rdparty/systemsemaphore/systemsemaphore.h"
#endif

#include "arguments.h"

class CleanerThread : public QThread
{
    Q_OBJECT

public:
    explicit CleanerThread(const ThreadData &data, QObject *parent = 0);
    void invokeStartNext(const StringPair &pair);
    void forceStop();

public slots:
    void exit();

signals:
    void cleaned(SVGInfo*);

private:
    bool m_isForceStop;
    ThreadData m_data;
    const QString m_zipPath;
    const QString m_cliPath;

#ifdef USE_IPC
    QSharedPointer<QProcess> m_proc;
    QSharedPointer<QSharedMemory> m_sharedMemory;
    QSharedPointer<SystemSemaphore> m_semaphore1;
    QSharedPointer<SystemSemaphore> m_semaphore2;
    bool m_isAcquiring;
#endif

#ifdef USE_IPC
    void initCleaner();
    void startProcess();
    void stopProcess();
    void writeToMemory(const QString &inFile, const QString &outFile);
    QString readMemory();
#endif
    void parseDefaultOutput(SVGInfo *info, const QString &outData);
    void unzip(const QString &inFile, const QString &outFile);
    void zip(const QString &outFile);

private slots:
    void startNext(const QString &inFile, const QString &outFile);
};

#endif // CLEANERTHREAD_H
