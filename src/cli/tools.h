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

#ifndef TOOLS_H
#define TOOLS_H

#include <QtDebug>

#include "enums.h"
#include "keys.h"

namespace Round {
    enum RoundType { Coordinate, Transform, Attribute };
}

bool isZero(qreal value);
bool isZeroTs(qreal value);
QString roundNumber(qreal value, Round::RoundType type = Round::Coordinate);
QString roundNumber(qreal value, int precision);
qreal getNum(const QChar *&str);
qreal strToDouble(const QString &str);
QString doubleToStr(const qreal value, int precision = 6);
bool isSpace(ushort ch);

class Tools
{
public:
    static QString convertUnitsToPx(const QString &text, qreal baseValue = 0);
    static QString trimColor(const QString &color);
    static QVariantHash initDefaultStyleHash();
    static int numbersBeforePoint(qreal value);
    static int zerosAfterPoint(qreal value);
    static bool isDigit(ushort ch);
    static qreal toDouble(const QChar *&str);

private:
    static QString replaceColorName(const QString &color);
};

#endif // TOOLS_H
