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

class TransformPrivate;

class Transform
{
public:
    Transform();
    Transform(const Transform &t);
    Transform &operator= (const Transform &t);
    bool operator== (const Transform &t) const;
    bool operator!= (const Transform &t) const;
    ~Transform();
    bool isNull() const;
    void clear();
    Transform clone() const;
    static Transform create();
    static Transform create(const QString &text);

    void append(const Transform &ts);
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
    TransformMatrix matrix() const;

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

protected:
    TransformPrivate *impl;
    Transform(TransformPrivate*);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Transform::Types)

#endif // TRANSFORM_H
