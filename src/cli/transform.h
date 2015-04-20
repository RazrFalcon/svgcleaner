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

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QList>
#include <QString>
#include <QRect>
#include <QSharedDataPointer>

class TransformMatrix;

class TransformPrivate : public QSharedData
{
public:
    TransformPrivate();

    QString simplified() const;
    TransformMatrix matrix() const;

    void divide(const QString &text);
    QList<TransformMatrix> parseTransform(const QString &text);
    QList<TransformMatrix> parseTransform(const QChar *str, int size);
    void calcMatrixes(const QString &text);
    void calcMatrixes(const QChar *str, int size);
    void calcParameters(const TransformMatrix &matrix);

    double oldX;
    double oldY;
    double xScale;
    double yScale;
    double xSkew;
    double ySkew;
    double angle;

    // matrix
    double a;
    double b;
    double c;
    double d;
    double e;
    double f;

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

    Types types;
    bool isChanged;
    QString lastSimplified;
};

class Transform
{
public:
    Transform() {}
    Transform(const QString &text);
    Transform(const QChar *str, int size);
    Transform(const Transform &t);
    bool operator== (const Transform &t) const;
    bool operator!= (const Transform &t) const;
    bool isNull() const;
    bool isValid() const;

    void append(const Transform &ts);
    void applyTranform(double &x, double &y) const;
    void applyTranform(double oldX, double oldY, double &newX, double &newY) const;
    void divide(const QString &text);
    QRectF transformRect(const QRectF &rect);
    QString simplified();
    QString simplified() const;
    double scaleFactor() const;
    bool isScale() const;
    bool isProportionalScale() const;
    bool isMirrored() const;
    bool isSkew() const;
    bool isRotating() const;
    bool isTranslate() const;
    TransformMatrix matrix() const;
    QString matrixString() const;
    TransformPrivate::Types type() const;

private:
    QSharedDataPointer<TransformPrivate> impl;
    Transform(TransformPrivate*);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TransformPrivate::Types)

QDebug operator<<(QDebug dbg, const Transform &t);

#endif // TRANSFORM_H
