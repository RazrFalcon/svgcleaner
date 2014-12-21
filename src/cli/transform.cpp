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
#include <qnumeric.h>

#include "tools.h"
#include "transform.h"

namespace TransformType {
    static const QString Matrix    = "matrix";
    static const QString Translate = "translate";
    static const QString Scale     = "scale";
    static const QString Rotate    = "rotate";
    static const QString SkewX     = "skewX";
    static const QString SkewY     = "skewY";
}


class TransformMatrix
{
public:
    explicit TransformMatrix();
    TransformMatrix(double a, double b, double c, double d, double e, double f);
    void setToIdentity();
    void invert();
    TransformMatrix operator *(const TransformMatrix &matrix) const;
    double& operator()(int row, int column);
    double operator()(int row, int column) const;

private:
    double m[3][3];

    static TransformMatrix subMatrix(const TransformMatrix &matrix, int n, int indRow, int indCol);
    double determinant();
    double _determinant(const TransformMatrix &matrix, int n);
};

TransformMatrix::TransformMatrix()
{
    setToIdentity();
}

TransformMatrix::TransformMatrix(double a, double b, double c, double d, double e, double f)
{
    setToIdentity();
    m[0][0] = a;
    m[1][0] = c;
    m[2][0] = e;

    m[0][1] = b;
    m[1][1] = d;
    m[2][1] = f;
}

void TransformMatrix::setToIdentity()
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

double TransformMatrix::determinant()
{
    return _determinant(*this, 3);
}

void TransformMatrix::invert()
{
    double det = determinant();
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

TransformMatrix TransformMatrix::operator *(const TransformMatrix &matrix) const
{
    TransformMatrix result;
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            double sum(0.0f);
            for (int j = 0; j < 3; ++j)
                sum += m[j][row] * matrix.m[col][j];
            result.m[col][row] = sum;
        }
    }
    return result;
}

double TransformMatrix::operator()(int row, int column) const
{
    Q_ASSERT(row >= 0 && row < 3 && column >= 0 && column < 3);
    return m[column][row];
}

TransformMatrix TransformMatrix::subMatrix(const TransformMatrix &matrix, int n, int indRow, int indCol)
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

double TransformMatrix::_determinant(const TransformMatrix &matrix, int n)
{
    double tmpDet = 0;
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

double &TransformMatrix::operator()(int row, int column)
{
    Q_ASSERT(row >= 0 && row < 3 && column >= 0 && column < 3);
    return m[column][row];
}


// http://www.w3.org/TR/SVG/coords.html#EstablishingANewUserSpace
Transform::Transform(const QString &text)
{
    if (text.isEmpty() || text.count(' ') == text.size())
        calcMatrixes("translate(0,0)");
    else
        calcMatrixes(text);
}

void Transform::setOldXY(double prevX, double prevY)
{
    oldX = prevX;
    oldY = prevY;
}

void Transform::divide(const QString &text)
{
    if (text.isEmpty() || text.count(' ') == text.size()) {
        divide("translate(0,0)");
        return;
    }
    QList<TransformMatrix> transMatrixList = parseTransform(text);

    TransformMatrix newMatrix = transMatrixList.at(0);
    for (int i = 1; i < transMatrixList.count(); ++i)
        newMatrix = newMatrix * transMatrixList.at(i);
    newMatrix.invert();
    TransformMatrix currMatrix(a,b,c,d,e,f);
    newMatrix = currMatrix * newMatrix;
    calcParameters(newMatrix);
}

QRectF Transform::transformRect(const QRectF &rect)
{
    QList<double> xList;
    xList.reserve(4);
    QList<double> yList;
    yList.reserve(4);

    setOldXY(rect.x(), rect.y());
    xList << newX();
    yList << newY();

    setOldXY(rect.x() + rect.width(), rect.y());
    xList << newX();
    yList << newY();

    setOldXY(rect.x(), rect.y() + rect.height());
    xList << newX();
    yList << newY();

    setOldXY(rect.x() + rect.width(), rect.y() + rect.height());
    xList << newX();
    yList << newY();

    double minx, miny, maxx, maxy;
    minx = maxx = xList.first();
    miny = maxy = yList.first();
    foreach (double x, xList) {
        if (x > maxx)
            maxx = x;
        else if (x < minx)
            minx = x;
    }
    foreach (double y, yList) {
        if (y > maxy)
            maxy = y;
        else if (y < miny)
            miny = y;
    }
    return QRectF(minx, miny, maxx - minx, maxy - miny);
}

double Transform::newX() const
{
    return a*oldX + c*oldY + e;
}

double Transform::newY() const
{
    return b*oldX + d*oldY + f;
}

QList<TransformMatrix> Transform::parseTransform(const QString &text)
{
    QList<TransformMatrix> list;
    StringWalker sw(text);
    while (!sw.atEnd()) {
        sw.skipSpaces();
        while (sw.current() == QL1C(','))
            sw.next();

        QString transformType;

        while (sw.current() != QL1C('(')) {
            if (sw.current() != QL1C(' '))
                transformType += sw.current();
            sw.next();
        }
        sw.next();

        double cx = 0;
        double cy = 0;
        TransformMatrix matrix;
        if (transformType == TransformType::Matrix) {
            matrix(0,0) = sw.number();
            matrix(1,0) = sw.number();
            matrix(0,1) = sw.number();
            matrix(1,1) = sw.number();
            matrix(0,2) = sw.number();
            matrix(1,2) = sw.number();
        } else if (transformType == TransformType::Translate) {
            matrix(0,2) = sw.number();
            sw.skipSpaces();
            if (sw.current() != QL1C(')'))
                matrix(1,2) = sw.number();
            else
                matrix(1,2) = 0;
        } else if (transformType == TransformType::Scale) {
            matrix(0,0) = sw.number();
            sw.skipSpaces();
            if (sw.current() != QL1C(')'))
                matrix(1,1) = sw.number();
            else
                matrix(1,1) = matrix(0,0);
        } else if (transformType == TransformType::Rotate) {
            double val = sw.number();
            cx = sw.number();
            cy = sw.number();
            matrix(0,0) = cos((val/180)*M_PI);
            matrix(1,0) = sin((val/180)*M_PI);
            matrix(0,1) = -sin((val/180)*M_PI);
            matrix(1,1) = cos((val/180)*M_PI);
        } else if (transformType == TransformType::SkewX) {
            matrix(0,1) = tan(sw.number());
        } else if (transformType == TransformType::SkewY) {
            matrix(1,0) = tan(sw.number());
        } else {
            qFatal("Error: wrong transform matrix: %s", qPrintable(text));
        }
        if (cx != 0 && cy != 0) {
            TransformMatrix matrix;
            matrix(0,2) = cx;
            matrix(1,2) = cy;
            list << matrix;
        }
        list << matrix;
        if (cx != 0 && cy != 0) {
            TransformMatrix matrix;
            matrix(0,2) = -cx;
            matrix(1,2) = -cy;
            list << matrix;
        }

        sw.jumpTo(QL1C(')'));
        if (sw.current() == QL1C(')'))
            sw.next();
        sw.skipSpaces();
    }
    return list;
}

void Transform::calcMatrixes(const QString &text)
{
    QList<TransformMatrix> transMatrixList = parseTransform(text);

    TransformMatrix newMatrix = transMatrixList.at(0);
    for (int i = 1; i < transMatrixList.count(); ++i)
        newMatrix = newMatrix * transMatrixList.at(i);
    calcParameters(newMatrix);
}

void Transform::calcParameters(TransformMatrix &matrix)
{
    a = b = c = d = e = f = 0;

    if (!isZeroTs(matrix(0,0)))
        a = matrix(0,0);
    if (!isZeroTs(matrix(1,0)))
        b = matrix(1,0);
    if (!isZeroTs(matrix(0,1)))
        c = matrix(0,1);
    if (!isZeroTs(matrix(1,1)))
        d = matrix(1,1);
    if (!isZero(matrix(0,2)))
        e = matrix(0,2);
    if (!isZero(matrix(1,2)))
        f = matrix(1,2);

//    qDebug() << "abc" << a << b << c << d << e << f;

    m_xScale = sqrt(a*a + c*c);
    m_yScale = sqrt(b*b + d*d);
    m_xSkew = atan(b)*(180.0/M_PI);
    m_ySkew = atan(c)*(180.0/M_PI);
    m_angle = atan(-b/a)*(180/M_PI);
    if (qIsNaN(m_angle))
        qFatal("Error: rotation is NaN");

    if (b < c)
        m_angle = -m_angle;

//    qDebug() << "calc" << m_xScale << m_yScale << m_xSkew << m_ySkew << m_angle;

    // detect all the transformations inside the matrix

    if (f != 0 || e != 0)
        m_types |= Translate;

    if ((isZeroTs(qAbs(b) - qAbs(c)) && b != 0) || !isZero(m_angle))
        m_types |= Rotate;

    if (m_xScale != 1 || m_yScale != 1)
        m_types |= Scale;

    if (isZeroTs(m_xScale - m_yScale))
        m_types |= ProportionalScale;

    if (m_xSkew != 0 || m_ySkew != 0)
        m_types |= Skew;

    if (m_xScale < 0 || a < 0)
        m_types |= HorizontalMirror;

    if (m_yScale < 0 || d < 0)
        m_types |= VertiacalMirror;
}

QString Transform::simplified() const
{
    // TODO: rewrite to QVarLengthArray

    if (!Keys::get().flag(Key::SimplifyTransformMatrix)) {
        QString ts = TransformType::Matrix + "(";
        ts += fromDouble(a, Round::Transform) + " ";
        ts += fromDouble(b, Round::Transform) + " ";
        ts += fromDouble(c, Round::Transform) + " ";
        ts += fromDouble(d, Round::Transform) + " ";
        ts += fromDouble(e, Round::Coordinate) + " ";
        ts += fromDouble(f, Round::Coordinate);
        ts += ")";
        return ts;
    }

    QString transform;
    QStringList newPoints;
    newPoints.reserve(2);

    Types type = m_types;
    type &= ~(Transform::ProportionalScale);

    // [1 0 0 1 tx ty] = translate
    if (type == Translate)
    {
        if (f != 0) {
            if (e == 0 && f == 0)
                return "";
            newPoints << fromDouble(e, Round::Coordinate);
            newPoints << fromDouble(f, Round::Coordinate);
        } else if (f == 0) {
            if (isZero(e))
                return "";
            newPoints << fromDouble(e, Round::Coordinate);
        }
        transform = TransformType::Translate;
    } // [sx 0 0 sy 0 0] = scale
    else if (type == Scale)
    {
        if (a != d) {
            if (isZeroTs(a) && isZeroTs(d))
                return "";
            newPoints << fromDouble(a, Round::Transform);
            newPoints << fromDouble(d, Round::Transform);
        } else {
            if (isZeroTs(a))
                return "";
            newPoints << fromDouble(a, Round::Transform);
        }
        transform = TransformType::Scale;
    } // [cos(a) sin(a) -sin(a) cos(a) 0 0] = rotate
    else if (type == Rotate)
    {
        if (m_angle == 0)
            return "";
        transform = TransformType::Rotate;
        newPoints << fromDouble(m_angle, Round::Transform);
    } // [1 0 tan(a) 1 0 0] = skewX, [1 tan(a) 0 1 0 0] = skewY
    else if (type == Skew)
    {
        if (m_xSkew == 0 && m_ySkew == 0)
            return "";
        if (m_xSkew != 0) {
            transform = TransformType::SkewX;
            newPoints << fromDouble(m_xSkew, Round::Transform);
        } else {
            transform = TransformType::SkewY;
            newPoints << fromDouble(m_ySkew, Round::Transform);
        }
    }
    else if (type == HorizontalMirror)
    {
        transform = TransformType::Scale;
        newPoints << "-1" << "1";
    }
    else if (type == VertiacalMirror)
    {
        transform = TransformType::Scale;
        newPoints << "1" << "-1";
    }
    else if (type == (HorizontalMirror | VertiacalMirror))
    {
        transform = TransformType::Scale;
        newPoints << "-1" << "-1";
    }
    else
    {
        if (   a == 1
            && b == 0
            && c == 0
            && d == 1
            && e == 0
            && f == 0)
            return "";

        transform = TransformType::Matrix;
        newPoints.reserve(6);
        newPoints << fromDouble(a, Round::Transform);
        newPoints << fromDouble(b, Round::Transform);
        newPoints << fromDouble(c, Round::Transform);
        newPoints << fromDouble(d, Round::Transform);
        newPoints << fromDouble(e, Round::Coordinate);
        newPoints << fromDouble(f, Round::Coordinate);
    }

    transform += "(";
    bool isTrim = Keys::get().flag(Key::RemoveUnneededSymbols);
    for (int i = 0; i < newPoints.size(); ++i) {
        if (i != 0) {
            if (isTrim) {
                if ((!newPoints.at(i-1).contains('.')
                     && newPoints.at(i).startsWith(QL1C('.')))
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

double Transform::scaleFactor() const
{
    return m_xScale;
}

bool Transform::isProportionalScale()
{
    return m_types.testFlag(ProportionalScale);
}

bool Transform::isMirrored()
{
    return m_types.testFlag(VertiacalMirror) || m_types.testFlag(HorizontalMirror);
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
