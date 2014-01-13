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

#include <cmath>

#include "tools.h"
#include "transform.h"

// http://www.w3.org/TR/SVG/coords.html#EstablishingANewUserSpace
Transform::Transform(const QString &text)
{
    Q_ASSERT(text.isEmpty() == false);
    if (text.isEmpty())
        return;
    m_points = mergeMatrixes(text);

    // calculate
    m_xScale = sqrt(pow(m_points.at(0), 2) + pow(m_points.at(2), 2));
    m_yScale = sqrt(pow(m_points.at(1), 2) + pow(m_points.at(3), 2));
}

void Transform::setOldXY(qreal prevX, qreal prevY)
{
    // NOTE: must be set
    m_baseX = prevX;
    m_baseY = prevY;
}

qreal Transform::newX() const
{
    return m_points.at(0)*m_baseX + m_points.at(2)*m_baseY + m_points.at(4);
}

qreal Transform::newY() const
{
    return m_points.at(1)*m_baseX + m_points.at(3)*m_baseY + m_points.at(5);
}

QList<TransformMatrix> Transform::parseTransform(const QStringRef &text)
{
    QList<TransformMatrix> list;
    const QChar *str = text.constData();
    const QChar *end = str + text.size();
    while (str != end) {
        while (str->isSpace())
            ++str;
        while (*str == ',')
            ++str;

        QString transformType;
        while (*str != QLatin1Char('(')) {
            if (*str != QLatin1Char(' '))
                transformType += *str;
            ++str;
        }
        ++str;

        TransformMatrix matrix;
        if (transformType == QL1S("matrix")) {
            matrix(0,0) = Tools::getNum(str);
            matrix(1,0) = Tools::getNum(str);
            matrix(0,1) = Tools::getNum(str);
            matrix(1,1) = Tools::getNum(str);
            matrix(0,2) = Tools::getNum(str);
            matrix(1,2) = Tools::getNum(str);
        } else if (transformType == QL1S("translate")) {
            matrix(0,2) = Tools::getNum(str);
            while (str->isSpace())
                ++str;
            if (*str != QLatin1Char(')'))
                matrix(1,2) = Tools::getNum(str);
            else
                matrix(1,2) = 0;
        } else if (transformType == QL1S("scale")) {
            matrix(0,0) = Tools::getNum(str);
            while (str->isSpace())
                ++str;
            if (*str != QLatin1Char(')'))
                matrix(1,1) = Tools::getNum(str);
            else
                matrix(1,1) = matrix(0,0);
        } else if (transformType == QL1S("rotate")) {
            qreal val = Tools::getNum(str);
            matrix(0,0) = cos((val/180)*M_PI);
            matrix(1,0) = sin((val/180)*M_PI);
            matrix(0,1) = -sin((val/180)*M_PI);
            matrix(1,1) = cos((val/180)*M_PI);
        } else if (transformType == QL1S("skewX")) {
            matrix(0,1) = tan(Tools::getNum(str));
        } else if (transformType == QL1S("skewY")) {
            matrix(1,0) = tan(Tools::getNum(str));
        } else {
            qFatal("Error: wrong transform matrix: %s", qPrintable(text.toString()));
        }
        list << matrix;

        while (*str != QLatin1Char(')'))
            ++str;
        if (*str == QLatin1Char(')'))
            ++str;
        while (str->isSpace())
            ++str;
    }
    return list;
}

QList<qreal> Transform::mergeMatrixes(QString text)
{
    QList<TransformMatrix> transMatrixList = parseTransform(text.midRef(0));

    TransformMatrix newMatrix = transMatrixList.at(0);
    for (int i = 1; i < transMatrixList.count(); ++i)
        newMatrix = newMatrix * transMatrixList.at(i);

    QList<qreal> pointList;
    pointList.reserve(6);
    pointList << newMatrix(0,0) << newMatrix(1,0) << newMatrix(0,1)
              << newMatrix(1,1) << newMatrix(0,2) << newMatrix(1,2);
    return pointList;
}

// TODO: add key for this
QString Transform::simplified() const
{
    if (m_points.isEmpty())
        return "";

    const QList<qreal> pt = m_points;
    QString transform;
    QStringList newPoints;
    newPoints.reserve(6);

    // [1 0 0 1 tx ty] = translate
    if (   pt.at(0) == 1
        && pt.at(1) == 0
        && pt.at(2) == 0
        && pt.at(3) == 1)
    {
        transform = "translate(";
        if (pt.at(5) != 0) {
            if (   Tools::isZero(pt.at(4))
                && Tools::isZero(pt.at(5)))
                return "";
            newPoints << Tools::roundNumber(pt.at(4), Tools::COORDINATE);
            newPoints << Tools::roundNumber(pt.at(5), Tools::COORDINATE);
        } else if (pt.at(4) != 0) {
            if (Tools::isZero(pt.at(4)))
                return "";
            newPoints << Tools::roundNumber(pt.at(4), Tools::COORDINATE);
        }
    } // [sx 0 0 sy 0 0] = scale
    else if (   pt.at(1) == 0
             && pt.at(2) == 0
             && pt.at(4) == 0
             && pt.at(5) == 0)
    {
        transform = "scale(";
        if (pt.at(0) != pt.at(3)) {
            if (   Tools::isZero(pt.at(0))
                && Tools::isZero(pt.at(3)))
                return "";
            newPoints << Tools::roundNumber(pt.at(0), Tools::TRANSFORM);
            newPoints << Tools::roundNumber(pt.at(3), Tools::TRANSFORM);
        } else {
            if (Tools::isZero(pt.at(0)))
                return "";
            newPoints << Tools::roundNumber(pt.at(0), Tools::TRANSFORM);
        }
    } // [cos(a) sin(a) -sin(a) cos(a) 0 0] = rotate
    else if (   pt.at(0) == pt.at(3)
             && pt.at(1) > 0
             && pt.at(2) < 0
             && pt.at(4) == 0
             && pt.at(5) == 0)
    {
        transform = "rotate(";
        qreal num = acos(pt.at(0))*(180/M_PI);
        if (Tools::isZero(num))
            return "";
        newPoints << Tools::roundNumber(num, Tools::TRANSFORM);
    } // [1 0 tan(a) 1 0 0] = skewX
    else if (   pt.at(0) == 1
             && pt.at(1) == 0
             && pt.at(3) == 1
             && pt.at(4) == 0
             && pt.at(5) == 0)
    {
        transform = "skewX(";
        qreal num = atan(pt.at(2))*(180/M_PI);
        if (Tools::isZero(pt.at(2)))
            return "";
        newPoints << Tools::roundNumber(num, Tools::TRANSFORM);
    } // [1 tan(a) 0 1 0 0] = skewY
    else if (   pt.at(0) == 1
             && pt.at(2) == 0
             && pt.at(3) == 1
             && pt.at(4) == 0
             && pt.at(5) == 0)
    {
        transform = "skewY(";
        qreal num = atan(pt.at(1))*(180/M_PI);
        if (Tools::isZero(num))
            return "";
        newPoints << Tools::roundNumber(num, Tools::TRANSFORM);
    } else {
        bool isAllZero = true;
        for (int i = 0; i < 6; ++i)
            isAllZero *= Tools::isZero(pt.at(i));
        if (isAllZero)
            return "";

        transform = "matrix(";
        for (int i = 0; i < 6; ++i) {
            if (i < 4)
                newPoints << Tools::roundNumber(pt.at(i), Tools::TRANSFORM);
            else
                newPoints << Tools::roundNumber(pt.at(i), Tools::COORDINATE);
        }
    }

    for (int i = 0; i < newPoints.size(); ++i) {
        if (i != 0) {
            if (Keys::get().flag(Key::RemoveUnneededSymbols)) {
                if ((!newPoints.at(i-1).contains('.')
                     && newPoints.at(i).startsWith(QLatin1Char('.')))
                        || newPoints.at(i).at(0).isDigit())
                    transform += " ";
            } else {
                transform += " ";
            }
        }
        transform += newPoints.at(i);
    }
    transform += ")";
    return transform;
}

qreal Transform::scaleFactor() const
{
    return m_xScale;
}

qreal Transform::xScale() const
{
    return m_xScale;
}

qreal Transform::yScale() const
{
    return m_yScale;
}

bool Transform::isProportionalScale()
{
    return Tools::isZero(m_xScale - m_yScale);
}

bool Transform::isMirrored()
{
    if (m_points.at(0) < 0)
        return true;
    else if (m_points.at(2) < 0)
        return true;
    return false;
}

bool Transform::isRotating()
{
    return !Tools::isZero(atan(m_points.at(1) / m_points.at(3)));
}
