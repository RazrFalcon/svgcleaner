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
    calcMatrixes(text);
}

void Transform::setOldXY(qreal prevX, qreal prevY)
{
    // NOTE: must be set
    oldX = prevX;
    oldY = prevY;
}

qreal Transform::newX() const
{
    return a*oldX + c*oldY + e;
}

qreal Transform::newY() const
{
    return b*oldX + d*oldY + f;
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

void Transform::calcMatrixes(const QString &text)
{
    QList<TransformMatrix> transMatrixList = parseTransform(text.midRef(0));

    TransformMatrix newMatrix = transMatrixList.at(0);
    for (int i = 1; i < transMatrixList.count(); ++i)
        newMatrix = newMatrix * transMatrixList.at(i);

    a = 0; b = 0; c = 0; d = 0; e = 0; f = 0;

    if (!Tools::isZeroTs(newMatrix(0,0)))
        a = newMatrix(0,0);
    if (!Tools::isZeroTs(newMatrix(1,0)))
        b = newMatrix(1,0);
    if (!Tools::isZeroTs(newMatrix(0,1)))
        c = newMatrix(0,1);
    if (!Tools::isZeroTs(newMatrix(1,1)))
        d = newMatrix(1,1);
    if (!Tools::isZero(newMatrix(0,2)))
        e = newMatrix(0,2);
    if (!Tools::isZero(newMatrix(1,2)))
        f = newMatrix(1,2);

    // calculate
    m_xScale = sqrt(pow(a, 2) + pow(c, 2));
    m_yScale = sqrt(pow(b, 2) + pow(d, 2));
    m_xSkew = atan(b)*(180.0/M_PI);
    m_ySkew = atan(c)*(180.0/M_PI);
    m_angle = acos(a)*(180/M_PI);
    if (b < c)
        m_angle = -m_angle;

    if (f != 0 || e != 0)
        m_types |= Translate;

    if (   (Tools::isZeroTs(qAbs(b) - qAbs(c)) && b != 0)
        || !Tools::isZero(m_angle))
        m_types |= Rotate;

    if (m_xScale != 1 || m_yScale != 1)
        m_types |= Scale;

    if (Tools::isZeroTs(m_xScale - m_yScale))
        m_types |= ProportionalScale;

    if (!Tools::isZeroTs(qAbs(m_xSkew) - qAbs(m_ySkew))
        && (m_xSkew != 0 || m_ySkew != 0))
        m_types |= Skew;

    if (m_xScale < 0 || m_yScale < 0 || a < 0 || d < 0)
        m_types |= Mirror;
}

// TODO: add key for this
QString Transform::simplified() const
{
    QString transform;
    QStringList newPoints;
    newPoints.reserve(2);

    Types type = m_types;
    type &= ~(Transform::Mirror | Transform::ProportionalScale);

    // [1 0 0 1 tx ty] = translate
    if (m_types == Translate)
    {
        if (f != e) {
            if (f == 0 && e == 0)
                return "";
            newPoints << Tools::roundNumber(e, Tools::COORDINATE);
            newPoints << Tools::roundNumber(f, Tools::COORDINATE);
        } else if (f == e || e == 0) {
            if (Tools::isZero(f))
                return "";
            newPoints << Tools::roundNumber(f, Tools::COORDINATE);
        }
        transform = "translate(";
    } // [sx 0 0 sy 0 0] = scale
    else if (m_types == Scale)
    {
        if (a != d) {
            if (   Tools::isZeroTs(a)
                && Tools::isZeroTs(d))
                return "";
            newPoints << Tools::roundNumber(a, Tools::TRANSFORM);
            newPoints << Tools::roundNumber(d, Tools::TRANSFORM);
        } else {
            if (Tools::isZeroTs(a))
                return "";
            newPoints << Tools::roundNumber(a, Tools::TRANSFORM);
        }
        transform = "scale(";
    } // [cos(a) sin(a) -sin(a) cos(a) 0 0] = rotate
    else if (m_types == Rotate)
    {
        if (m_angle == 0)
            return "";
        transform = "rotate(";
        newPoints << Tools::roundNumber(m_angle, Tools::TRANSFORM);
    } // [1 0 tan(a) 1 0 0] = skewX, [1 tan(a) 0 1 0 0] = skewY
    else if (m_types == Skew)
    {
        if (m_xSkew == 0 && m_ySkew == 0)
            return "";
        if (m_xSkew != 0) {
            transform = "skewX(";
            newPoints << Tools::roundNumber(m_xSkew, Tools::TRANSFORM);
        } else {
            transform = "skewY(";
            newPoints << Tools::roundNumber(m_ySkew, Tools::TRANSFORM);
        }
    } else {
        if (   a == 1
            && b == 0
            && c == 0
            && d == 1
            && e == 0
            && f == 0)
            return "";

        transform = "matrix(";
        newPoints.reserve(6);
        newPoints << Tools::roundNumber(a, Tools::TRANSFORM);
        newPoints << Tools::roundNumber(b, Tools::TRANSFORM);
        newPoints << Tools::roundNumber(c, Tools::TRANSFORM);
        newPoints << Tools::roundNumber(d, Tools::TRANSFORM);
        newPoints << Tools::roundNumber(e, Tools::COORDINATE);
        newPoints << Tools::roundNumber(f, Tools::COORDINATE);
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

bool Transform::isProportionalScale()
{
    return m_types.testFlag(ProportionalScale);
}

bool Transform::isMirrored()
{
    return m_types.testFlag(Mirror);
}

bool Transform::isSkew()
{
    return m_types.testFlag(Skew);
}

bool Transform::isRotating()
{
    return m_types.testFlag(Rotate);
}

bool Transform::isTranslate()
{
    return m_types.testFlag(Translate);
}

Transform::Types Transform::type()
{
    return m_types;
}
