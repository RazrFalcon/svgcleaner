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

#include <cmath>

class TransformMatrix
{
public:
    explicit TransformMatrix() { setToIdentity(); }
    TransformMatrix(qreal a, qreal b, qreal c, qreal d, qreal e, qreal f)
    {
        setToIdentity();
        m[0][0] = a;
        m[1][0] = c;
        m[2][0] = e;

        m[0][1] = b;
        m[1][1] = d;
        m[2][1] = f;
    }

    void setToIdentity()
    {
        for (int col = 0; col < 3; ++col) {
            for (int row = 0; row < 3; ++row) {
                if (row == col)
                    m[col][row] = 1.0f;
                else
                    m[col][row] = 0.0f;
            }
        }
    }

    qreal determinant()
    {
        return _determinant(*this, 3);
    }

    void invert()
    {
        qreal det = determinant();
        TransformMatrix invMat;
        const int n = 3;
        for(int i = 0; i < n; ++i) {
            for(int j = 0; j < n; ++j) {
                int mm = n - 1;
                invMat(j,i) = pow(-1.0, i + j + 2) * _determinant(subMatrix(*this, n, i, j), mm) / det;
            }
        }
        *this = invMat;
    }

    TransformMatrix operator *(const TransformMatrix &matrix)
    {
        TransformMatrix result;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                qreal sum(0.0f);
                for (int j = 0; j < 3; ++j)
                    sum += m[j][row] * matrix.m[col][j];
                result.m[col][row] = sum;
            }
        }
        return result;
    }

    qreal& operator()(int row, int column)
    {
        Q_ASSERT(row >= 0 && row < 3 && column >= 0 && column < 3);
        return m[column][row];
    }

    qreal operator()(int row, int column) const
    {
        Q_ASSERT(row >= 0 && row < 3 && column >= 0 && column < 3);
        return m[column][row];
    }

private:
    qreal m[3][3];

    TransformMatrix subMatrix(const TransformMatrix &matrix, int n, int indRow, int indCol)
    {
        TransformMatrix tmpMatrix;
        int ki = 0;
        for (int i = 0; i < n; i++) {
            if (i != indRow) {
                for (int j = 0, kj = 0; j < n; ++j){
                    if (j != indCol) {
                        tmpMatrix(ki,kj) = matrix(i,j);
                        kj++;
                    }
                }
                ki++;
            }
        }
        return tmpMatrix;
    }

    qreal _determinant(const TransformMatrix &matrix, int n)
    {
        qreal tmpDet = 0;
        if (n == 1) {
            tmpDet = matrix(0,0);
        } else if (n == 2) {
            tmpDet = matrix(0,0) * matrix(1,1) - matrix(1,0) * matrix(0,1);
        } else {
            double k = 1;
            for (int i = 0; i < n; i++) {
                int mm = n - 1;
                tmpDet = tmpDet + k * matrix(0,i) * _determinant(subMatrix(matrix, n, 0, i), mm);
                k = -k;
            }
        }
        return tmpDet;
    }
};

class Transform
{
public:
    explicit Transform(const QString &text);
    void setOldXY(qreal prevX, qreal prevY);
    void divide(const QString &text);
    QRectF transformRect(const QRectF &rect);
    qreal newX() const;
    qreal newY() const;
    QString simplified() const;
    qreal scaleFactor() const;
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

private:
    qreal oldX;
    qreal oldY;
    qreal m_xScale;
    qreal m_yScale;
    qreal m_xSkew;
    qreal m_ySkew;
    qreal m_angle;

    qreal a;
    qreal b;
    qreal c;
    qreal d;
    qreal e;
    qreal f;

    Types m_types;

    QList<TransformMatrix> parseTransform(const QStringRef &text);
    void calcMatrixes(const QString &text);
    void calcParameters(TransformMatrix &matrix);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Transform::Types)

#endif // TRANSFORM_H
