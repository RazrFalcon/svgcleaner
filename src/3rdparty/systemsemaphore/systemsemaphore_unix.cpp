/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QCoreApplication>
#include <QFile>
#include <QtDebug>

#include "systemsemaphore.h"
#include "systemsemaphore_p.h"

#include <errno.h>
#include <fcntl.h>

#define EINTR_LOOP(var, cmd)                \
do {                                        \
    var = cmd;                              \
    } while (var == -1 && errno == EINTR)


SystemSemaphorePrivate::SystemSemaphorePrivate() :
    isTimeout(false),
    semaphore(SEM_FAILED),
    createdSemaphore(false)
{
}

bool SystemSemaphorePrivate::handle(SystemSemaphore::AccessMode mode)
{
    if (semaphore != SEM_FAILED)
        return true;  // we already have a semaphore

    if (fileName.isEmpty()) {
        return false;
    }

    QByteArray semName = QFile::encodeName(fileName);

    // Always try with O_EXCL so we know whether we created the semaphore.
    int oflag = O_CREAT | O_EXCL;
    for (int tryNum = 0, maxTries = 1; tryNum < maxTries; ++tryNum) {
        do {
            semaphore = sem_open(semName.constData(), oflag, 0666, initialValue);
        } while (semaphore == SEM_FAILED && errno == EINTR);
        if (semaphore == SEM_FAILED && errno == EEXIST) {
            if (mode == SystemSemaphore::Create) {
                if (sem_unlink(semName.constData()) == -1 && errno != ENOENT) {
                    return false;
                }
                // Race condition: the semaphore might be recreated before
                // we call sem_open again, so we'll retry several times.
                maxTries = 3;
            } else {
                // Race condition: if it no longer exists at the next sem_open
                // call, we won't realize we created it, so we'll leak it later.
                oflag &= ~O_EXCL;
                maxTries = 2;
            }
        } else {
            break;
        }
    }
    if (semaphore == SEM_FAILED) {
        return false;
    }

    createdSemaphore = (oflag & O_EXCL) != 0;
    return true;
}

void SystemSemaphorePrivate::cleanHandle()
{
    if (semaphore != SEM_FAILED) {
        sem_close(semaphore);
        semaphore = SEM_FAILED;
    }

    if (createdSemaphore) {
        sem_unlink(QFile::encodeName(fileName).constData());
        createdSemaphore = false;
    }
}

bool SystemSemaphorePrivate::modifySemaphore(int count)
{
    if (!handle())
        return false;

    if (count > 0) {
        int cnt = count;
        do {
            if (sem_post(semaphore) == -1) {
                // rollback changes to preserve the SysV semaphore behavior
                for ( ; cnt < count; ++cnt) {
                    register int res;
                    EINTR_LOOP(res, sem_wait(semaphore));
                }
                return false;
            }
            --cnt;
        } while (cnt > 0);
    } else {
        register int res;
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 60; // 60 sec
        EINTR_LOOP(res, sem_timedwait(semaphore, &ts));
        isTimeout = (errno == ETIMEDOUT);
        if (res == -1) {
            // If the semaphore was removed be nice and create it and then modifySemaphore again
            if (errno == EINVAL || errno == EIDRM) {
                semaphore = SEM_FAILED;
                return modifySemaphore(count);
            }
            return false;
        }
    }

    return true;
}
