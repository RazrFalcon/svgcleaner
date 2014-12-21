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

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QList>
#include <QString>
#include <QRect>

class TransformMatrix;

class Transform
{
public:
    explicit Transform(const QString &text);
    void setOldXY(double prevX, double prevY);
    void divide(const QString &text);
    QRectF transformRect(const QRectF &rect);
    double newX() const;
    double newY() const;
    QString simplified() const;
    double scaleFactor() const;
    bool isProportionalScale();
    bool isMirrored();
    bool isSkew();
    bool isRotating();
    bool isTranslate();

    enum TsType {
        Scale = 0x1,
        Rotate = 0x2,
        Skew = 0x4,
        Translate = 0x8,
        HorizontalMirror = 0x10,
        VertiacalMirror = 0x20,
        ProportionalScale = 0x40
    };
    Q_DECLARE_FLAGS(Types, TsType)
    Types type();

    bool operator ==(const Transform &t) {
        return (   a == t.a
                && b == t.b
                && c == t.c
                && d == t.d
                && e == t.e
                && f == t.f);
    }

private:
    double oldX;
    double oldY;
    double m_xScale;
    double m_yScale;
    double m_xSkew;
    double m_ySkew;
    double m_angle;

    double a;
    double b;
    double c;
    double d;
    double e;
    double f;

    Types m_types;

    QList<TransformMatrix> parseTransform(const QString &text);
    void calcMatrixes(const QString &text);
    void calcParameters(TransformMatrix &matrix);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Transform::Types)

#endif // TRANSFORM_H
