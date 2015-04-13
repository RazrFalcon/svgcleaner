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

#ifndef STRINGWALKER_H
#define STRINGWALKER_H

#include <QString>

class StringWalker
{
public:
    enum Opt { NoSkip, SkipComma };

    StringWalker(const QString &text);
    StringWalker(const QChar *astr, int size);
    int jumpTo(const QChar &c);
    int jumpToSpace();
    QString readBefore(int len) const;
    uint readBeforeId(int len) const;
    void next();
    void next(int count);
    void skipSpaces();
    bool atEnd() const;
    bool isValid() const;
    const QChar& current() const;
    double number(Opt opt = SkipComma, bool *ok = 0);
    static bool isSpace(ushort ch);

private:
    const QChar *str;
    const QChar *end;

    static bool isDigit(ushort ch);
};

#endif // STRINGWALKER_H
