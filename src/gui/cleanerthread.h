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

#ifndef CLEANERTHREAD_H
#define CLEANERTHREAD_H

#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtCore/QTime>

#include "arguments.h"

class CleanerThread : public QObject
{
    Q_OBJECT

public:
    explicit CleanerThread(ToThread args, QObject *parent = 0);

public slots:
    void startNext(const QString &inPath, const QString &outPath);

signals:
    void cleaned(const SVGInfo);
//    void criticalError(QString);

private:
    QProcess *proc;
    QString currentIn;
    QString currentOut;
    QString outSVG;
    QString scriptOutput;
//    QString scriptErrors;
    QTime cleaningTime;
    ToThread arguments;
    int findValue(const QString &text);
    void unzip(const QString &inPath);
    SVGInfo info();

private slots:
    void finished(int);
    void readyRead();
//    void readyReadError();
};

#endif // CLEANERTHREAD_H
