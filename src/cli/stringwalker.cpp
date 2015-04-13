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

#include "mindef.h"
#include "stringwalker.h"

// Simple 'const QChar *' wrapper

Q_CORE_EXPORT double qstrtod(const char *s00, char const **se, bool *ok);

StringWalker::StringWalker(const QString &text)
{
    str = text.constData();
    end = str + text.size();
}

StringWalker::StringWalker(const QChar *astr, int size)
{
    str = astr;
    end = str + size;
}

int StringWalker::jumpTo(const QChar &c)
{
    int len = 0;
    while (!atEnd() && *str != c) {
        len++;
        str++;
    }
    return len;
}

int StringWalker::jumpToSpace()
{
    int len = 0;
    while (!isSpace(str->unicode())) {
        str++;
        len++;
    }
    return len;
}

QString StringWalker::readBefore(int len) const
{
    return QString(str - len, len);
}

uint StringWalker::readBeforeId(int len) const
{
    return hash(str - len, len);
}

void StringWalker::next()
{
    str++;
}

void StringWalker::next(int count)
{
    for (int i = 0; i < count; ++i)
        str++;
}

void StringWalker::skipSpaces()
{
    while (isSpace(str->unicode()))
        str++;
}

bool StringWalker::atEnd() const
{
    return (str == end);
}

bool StringWalker::isValid() const
{
    return str;
}

const QChar &StringWalker::current() const
{
    return *str;
}

// the number() code underneath is from QtSvg module (qsvghandler.cpp) (LGPLv2 license)
double StringWalker::number(Opt opt, bool *ok)
{
    skipSpaces();

    const int maxLen = 255; // technically doubles can go til 308+ but whatever
    char temp[maxLen+1];
    int pos = 0;

    if (*str == QL1C('-')) {
        temp[pos++] = '-';
        ++str;
    } else if (*str == QL1C('+')) {
        ++str;
    }
    while (isDigit(str->unicode()) && pos < maxLen) {
        temp[pos++] = str->unicode();
        ++str;
    }
    if (*str == QL1C('.') && pos < maxLen) {
        temp[pos++] = '.';
        ++str;
    }
    while (isDigit(str->unicode()) && pos < maxLen) {
        temp[pos++] = str->unicode();
        ++str;
    }
    bool exponent = false;
    if ((*str == QL1C('e') || *str == QL1C('E')) && pos < maxLen) {
        exponent = true;
        temp[pos++] = 'e';
        ++str;
        if ((*str == QL1C('-') || *str == QL1C('+')) && pos < maxLen) {
            temp[pos++] = str->unicode();
            ++str;
        }
        while (isDigit(str->unicode()) && pos < maxLen) {
            temp[pos++] = str->unicode();
            ++str;
        }
    }

    if (ok)
        *ok = (str == end);

    temp[pos] = '\0';

    double val;
    if (!exponent && pos <= 10) {
        int ival = 0;
        const char *t = temp;
        bool neg = false;
        if (*t == '-') {
            neg = true;
            ++t;
        }
        while (*t && *t != '.') {
            ival *= 10;
            ival += (*t) - '0';
            ++t;
        }
        if (*t == '.') {
            ++t;
            int div = 1;
            while (*t) {
                ival *= 10;
                ival += (*t) - '0';
                div *= 10;
                ++t;
            }
            val = ((double)ival)/((double)div);
        } else {
            val = ival;
        }
        if (neg)
            val = -val;
    } else {
        val = qstrtod(temp, 0, 0);
    }

    skipSpaces();
    if (opt == SkipComma && *str == QL1C(','))
        ++str;
    return val;
}

    // check is space or non printable character
bool StringWalker::isSpace(ushort ch)
{
    // '32' is UTF-8 space
    if (ch == 32)
        return true;

    // '9'  is character tabulation
    // '10' is line feed (LF)
    // '11' is line tabulation
    // '12' is form feed (FF)
    // '13' is carriage return (CR)
    if (ch >= 9 && ch <= 13)
        return true;
    return false;
}

// the isDigit code underneath is from QtSvg module (qsvghandler.cpp) (LGPLv2 license)
// '0' is 0x30 and '9' is 0x39
bool StringWalker::isDigit(ushort ch)
{
    static quint16 magic = 0x3ff;
    return ((ch >> 4) == 3) && (magic >> (ch & 15));
}

