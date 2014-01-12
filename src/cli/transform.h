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

class TransformMatrix
{
public:
    TransformMatrix() { setToIdentity(); }
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
private:
    qreal m[3][3];
};

class Transform
{
public:
    explicit Transform(const QString &text);
    void setOldXY(qreal prevX, qreal prevY);
    qreal newX() const;
    qreal newY() const;
    QString simplified() const;
    qreal scaleFactor() const;
    qreal xScale() const;
    qreal yScale() const;
    bool isProportionalScale();
    bool isMirrored();
    bool isRotating();

private:
    QList<qreal> m_points;
    qreal m_baseX;
    qreal m_baseY;
    qreal m_xScale;
    qreal m_yScale;

    QList<TransformMatrix> parseTransform(const QStringRef &text);
    QList<qreal> mergeMatrixes(QString text);
};

#endif // TRANSFORM_H
