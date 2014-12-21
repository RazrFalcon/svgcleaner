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
#include <QVarLengthArray>

#include "enums.h"
#include "keys.h"

// we cannot use static variables while unit testing
// so replace 'static' with nothing
#ifdef U_TEST
# define u_static
#else
# define u_static static
#endif

namespace Round {
    enum RoundType { Coordinate, Transform, Attribute };
}

bool isZero(double value);
bool isZeroTs(double value);
QString fromDouble(double value, Round::RoundType type = Round::Coordinate);
QString fromDouble(double value, int precision);
double toDouble(const QString &str);
void doubleToVarArr(QVarLengthArray<ushort> &arr, double value, int precision = 6);
bool isSpace(ushort ch);

class Tools
{
public:
    static QString convertUnitsToPx(const QString &text, double baseValue = 0);
    static QString trimColor(const QString &color);
    static QVariantHash initDefaultStyleHash();
    static int numbersBeforePoint(double value);
    static int zerosAfterPoint(double value);

private:
    static QString replaceColorName(const QString &color);
};

class StringWalker
{
public:
    enum Opt { NoSkip, SkipComma };

    StringWalker(const QString &text);
    int jumpTo(const QChar &c);
    int jumpToSpace();
    QString readBefore(int len) const;
    void next();
    void next(int count);
    void skipSpaces();
    bool atEnd() const;
    bool isValid() const;
    QChar current() const;
    const QChar *&data();
    double number(Opt opt = SkipComma);

private:
    const QChar *str;
    const QChar *end;

    static bool isDigit(ushort ch);
};

#endif // TOOLS_H
