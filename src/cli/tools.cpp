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

#include <cmath>

#include "stringwalker.h"
#include "enums.h"
#include "tools.h"

bool isZero(double value)
{
    u_static const double minValue = 1.0 / pow(10, Keys::get().coordinatesPrecision());
    return (qAbs(value) < minValue);
}

bool isZeroTs(double value)
{
    u_static const double minValue = 1.0 / pow(10, Keys::get().transformPrecision());
    return (qAbs(value) < minValue);
}

QString fromDouble(double value, Round::RoundType type)
{
    return fromDouble(value, Keys::get().precision(type));
}

QString fromDouble(double value, int precision)
{
    CharArray array;
    doubleToVarArr(array, value, precision);
    return QString(reinterpret_cast<QChar *>(array.data()), array.size());
}

void doubleToVarArr(CharArray &arr, double value, int precision)
{
    double fractpart, intpart;
    fractpart = modf(value, &intpart);

    // round number when fraction part is really small
    // when fraction part is smaller than 1% of integer part
    // 24.2008 -> 24.2
    // 2.01738 -> 2.02
    // 3.004   -> 3
    if (qAbs(fractpart/intpart*100) < 1.1f) {
        double v = pow(10, (precision-1));
        double fractpart2 = qRound(fractpart * v) / v;
        value = intpart + fractpart2;
    }

    uint multiplier = 1;
    while (precision--)
        multiplier *= 10;
    double tmpValue = qRound64(qAbs(value) * multiplier);

    static const ushort zero = '0';
    if (qFuzzyIsNull(tmpValue)) {
        arr.append(zero);
        return;
    }

    double newValue = tmpValue/multiplier;

    int decimalPointPos = 0;
    {
        int v = floor(newValue);
        while(v) {
            v /= 10;
            decimalPointPos++;
        }
    }
    int zeroAfterPoint = 0;
    {
        double v = qAbs(newValue);
        while(v < 0.1) {
            v *= 10;
            zeroAfterPoint++;
        }
    }

    qulonglong l = tmpValue;
    ushort buff[65];
    ushort *p = buff + 65;
    static const ushort point = '.';
    static const ushort sign  = '-';
    int pos = 0;
    while (l != 0) {
        pos++;
        l /= 10;
    }
    l = tmpValue;
    bool isTrailingZero = true;
    int charCount = 0;
    while (l != 0) {
        int c = l % 10;
        if (c != 0)
            isTrailingZero = false;
        if (!isTrailingZero) {
            *(--p) = zero + c;
            charCount++;
        }
        pos--;
        if (pos == decimalPointPos && decimalPointPos != 0) {
            if (charCount > 0)
                *(--p) = point;
            isTrailingZero = false;
        }
        l /= 10;
    }
    while (zeroAfterPoint--)
        *(--p) = zero;
    if (decimalPointPos == 0) {
        *(--p) = point;
        u_static const bool useLeadingZero = !Keys::get().flag(Key::RemoveUnneededSymbols);
        if (useLeadingZero)
            *(--p) = zero;
    }
    if (value < 0)
        *(--p) = sign;

    arr.append(p, 65 - (p - buff));
}

double toDouble(const QString &str, bool *ok)
{
    return StringWalker(str).number(StringWalker::NoSkip, ok);
}

// http://www.w3.org/TR/SVG11/coords.html#Units
QString Tools::convertUnitsToPx(const QString &text, double baseValue)
{
    QString unit;
    double number = 0;
    StringWalker sw(text);
    while (!sw.atEnd()) {
        number = sw.number();
        while ((sw.current().isLetter() || sw.current() == LengthType::Percent) && !sw.atEnd()) {
            unit += sw.current();
            sw.next();
        }
    }

    if (unit == LengthType::px)
        return fromDouble(number, Round::Attribute);

    // fix string parsing, getNum func detect 'e' char as exponent...
    if (unit == QL1S("x"))
        unit = LengthType::ex;
    else if (unit == QL1S("m"))
        unit = LengthType::em;

    // note that all relative units depends on users screen dpi
    // and SVG Cleaner use 90dpi as default

    if (unit == LengthType::pt)
        number = number * 1.25;
    else if (unit == LengthType::pc)
        number = number * 15;
    else if (unit == LengthType::mm)
        number = number * 3.543307;
    else if (unit == LengthType::cm)
        number = number * 35.43307;
    else if (unit == LengthType::in)
        number = number * 90;
    else if (unit == LengthType::Percent)
        number = number * baseValue / 100;
    else if (unit == LengthType::em)
        number = number * baseValue;
    else if (unit == LengthType::ex)
        number = number * baseValue / 2;
    else
        return text;

    return fromDouble(number, Round::Attribute);
}
