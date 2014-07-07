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

#ifndef SYSTEMSEMAPHORE_P_H
#define SYSTEMSEMAPHORE_P_H

#include <QRegExp>
#include <QCryptographicHash>
#include <QDir>

#ifdef Q_OS_LINUX
    #include <semaphore.h>
#elif defined(Q_OS_WIN)
    #include <windows.h>
#endif

#include "systemsemaphore.h"

class SystemSemaphorePrivate
{
public:
    SystemSemaphorePrivate();

    inline QString makeKeyFileName() const
    {
        if (key.isEmpty())
            return QString();

        QString result = QLatin1String("qipc_systemsem_");

        QString part1 = key;
        part1.replace(QRegExp(QLatin1String("[^A-Za-z]")), QString());
        result.append(part1);

        QByteArray hex = QCryptographicHash::hash(key.toUtf8(), QCryptographicHash::Sha1).toHex();
        result.append(QLatin1String(hex));
    #ifdef Q_OS_WIN
        return result;
    #elif defined(Q_OS_LINUX)
        return QLatin1Char('/') + result;
    #endif
        return QString();
    }

#ifdef Q_OS_WIN
    HANDLE handle(SystemSemaphore::AccessMode mode = SystemSemaphore::Open);
#elif defined(Q_OS_LINUX)
    bool handle(SystemSemaphore::AccessMode mode = SystemSemaphore::Open);
#endif
    void cleanHandle();
    bool modifySemaphore(int count);

    bool isTimeout;
    QString key;
    QString fileName;
    int initialValue;

#ifdef Q_OS_WIN
    HANDLE semaphore;
    HANDLE semaphoreLock;
#elif defined(Q_OS_LINUX)
    sem_t *semaphore;
    bool createdSemaphore;
#endif
};

#endif // SYSTEMSEMAPHORE_P_H
