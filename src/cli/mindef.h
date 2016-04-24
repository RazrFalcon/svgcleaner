/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2012-2015 Evgeniy Reizner
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

#ifndef MINDEF_H
#define MINDEF_H

#include <QSet>

#if QT_VERSION >= 0x050000
#include <QMap>
#endif

#define QL1S(x) QLatin1String(x)
#define QL1C(x) QLatin1Char(x)

#define CheckData(_retValue) if (!impl) return _retValue

typedef QSet<QString> StringSet;
typedef QHash<uint,QString> IntHash;
typedef QHash<QString,QString> StringHash;
typedef QMap<QString,QString> StringMap;

uint hash(const QChar *p, int n);

#include <QtGlobal>
#if QT_VERSION >= 0x050000
    uint qHash(const QString&, uint seed);
#else
    uint qHash(const QString &key);
#endif

// we cannot use static variables while unit testing
// so replace 'static' with nothing
#ifdef U_TEST
# define u_static
#else
# define u_static static
#endif

#endif // MINDEF_H
