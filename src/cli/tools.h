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

#include <QVariantHash>

#include "keys.h"

#define QL1S(x) QLatin1String(x)

class Tools
{
public:
    explicit Tools() {}
    enum RoundType { COORDINATE, TRANSFORM, ATTRIBUTE };
    static bool isZero(qreal value);
    static bool isZeroTs(qreal value);
    static QString convertUnitsToPx(const QString &text, qreal baseValue = 0);
    static QString roundNumber(qreal value, RoundType type = COORDINATE);
    static QString roundNumber(qreal value, int precision);
    static QString trimColor(const QString &color);
    static QVariantHash initDefaultStyleHash();
    static StringHash splitStyle(const QString &style);
    static QString removeEdgeSpaces(const QString &str);
    static qreal getNum(const QChar *&str);
    static qreal strToDouble(const QString &str);
    static QString doubleToStr(const qreal value, int precision = 6);

private:
    static bool isDigit(ushort ch);
    static qreal toDouble(const QChar *&str);
    static QString replaceColorName(const QString &color);
    static int numbersBeforePoint(qreal value);
    static int zerosAfterPoint(qreal value);
};

// TODO: maybe add namespace with static string with usual elem/attr names,
// to prevent QString::fromLatin1_helper executing

namespace Props {
    extern const StringSet fillList;
    extern const StringSet strokeList;
    extern const StringSet presentationAttributes;
    extern const CharList linkableStyleAttributes;
    extern const QStringList linearGradient;
    extern const QStringList radialGradient;
    extern const QStringList filter;
    extern const StringSet maskAttributes;
    extern const StringSet digitList;
    extern const StringSet filterDigitList;
    extern const StringSet defsList;
    extern const StringSet referencedElements;
    extern const StringSet textElements;
    extern const StringSet textAttributes;
    extern const QVariantHash defaultStyleValues;
    extern const StringSet svgElementList;
    extern const StringSet elementsUsingXLink;
    extern const StringSet containers;
    extern const StringSet stopAttributes;
    extern const StringSet lengthTypes;
}

namespace CleanerAttr {
    extern const char * const UsedElement;
    extern const char * const BoundingBox;
    extern const char * const BBoxTransform;
}

#endif // TOOLS_H
