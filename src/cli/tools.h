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

#ifndef TOOLS_H
#define TOOLS_H

#include <QtDebug>
#include <QVarLengthArray>

#include "keys.h"

typedef QVarLengthArray<ushort,65> CharArray;

bool isZero(double value);
bool isZeroTs(double value);
QString fromDouble(double value, Round::RoundType type = Round::Coordinate);
QString fromDouble(double value, int precision);
double toDouble(const QString &str, bool *ok = 0);
void doubleToVarArr(CharArray &arr, double value, int precision = 6);

class Tools
{
public:
    static QString convertUnitsToPx(const QString &text, double baseValue = 0);
};

#endif // TOOLS_H
