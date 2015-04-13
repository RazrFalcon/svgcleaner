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

#include <cmath>
#include <qnumeric.h>

#include "stringwalker.h"
#include "mindef.h"
#include "tools.h"
#include "transform.h"

namespace TransformType {
    static const QString Matrix    = QL1S("matrix");
    static const QString Translate = QL1S("translate");
    static const QString Scale     = QL1S("scale");
    static const QString Rotate    = QL1S("rotate");
    static const QString SkewX     = QL1S("skewX");
    static const QString SkewY     = QL1S("skewY");
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

// SVG 2D transform implementation

// http://www.w3.org/TR/SVG/coords.html#EstablishingANewUserSpace

TransformPrivate::TransformPrivate() : QSharedData(), isChanged(true)
{
}

QString TransformPrivate::simplified() const
{
    if (!Keys::get().flag(Key::SimplifyTransformMatrix)) {
        QString ts = TransformType::Matrix + QL1S("(");
        ts += fromDouble(a, Round::Transform) + QL1S(" ");
        ts += fromDouble(b, Round::Transform) + QL1S(" ");
        ts += fromDouble(c, Round::Transform) + QL1S(" ");
        ts += fromDouble(d, Round::Transform) + QL1S(" ");
        ts += fromDouble(e, Round::Coordinate) + QL1S(" ");
        ts += fromDouble(f, Round::Coordinate);
        ts += QL1S(")");
        return ts;
    }

    QString transform;
    QStringList newPoints;
    newPoints.reserve(2);

    Types type = types;
    type &= ~(ProportionalScale);

    // [1 0 0 1 tx ty] = translate
    if (type == Translate)
    {
        if (f != 0) {
            if (e == 0 && f == 0)
                return QString();
            newPoints << fromDouble(e, Round::Coordinate);
            newPoints << fromDouble(f, Round::Coordinate);
        } else if (f == 0) {
            if (isZero(e))
                return QString();
            newPoints << fromDouble(e, Round::Coordinate);
        }
        transform = TransformType::Translate;
    } // [sx 0 0 sy 0 0] = scale
    else if (type == Scale)
    {
        if (a != d) {
            if (isZeroTs(a) && isZeroTs(d))
                return QString();
            newPoints << fromDouble(a, Round::Transform);
            newPoints << fromDouble(d, Round::Transform);
        } else {
            if (isZeroTs(a))
                return QString();
            newPoints << fromDouble(a, Round::Transform);
        }
        transform = TransformType::Scale;
    } // [cos(a) sin(a) -sin(a) cos(a) 0 0] = rotate
    else if (type == Rotate)
    {
        if (angle == 0)
            return QString();
        transform = TransformType::Rotate;
        newPoints << fromDouble(angle, Round::Transform);
    } // [1 0 tan(a) 1 0 0] = skewX, [1 tan(a) 0 1 0 0] = skewY
    else if (type == Skew)
    {
        if (xSkew == 0 && ySkew == 0)
            return QString();
        if (xSkew != 0) {
            transform = TransformType::SkewX;
            newPoints << fromDouble(xSkew, Round::Transform);
        } else {
            transform = TransformType::SkewY;
            newPoints << fromDouble(ySkew, Round::Transform);
        }
    }
    else if (type == HorizontalMirror)
    {
        transform = TransformType::Scale;
        newPoints << QL1S("-1") << QL1S("1");
    }
    else if (type == VertiacalMirror)
    {
        transform = TransformType::Scale;
        newPoints << QL1S("1") << QL1S("-1");
    }
    else if (type == (HorizontalMirror | VertiacalMirror))
    {
        transform = TransformType::Scale;
        newPoints << QL1S("-1") << QL1S("-1");
    }
    else
    {
        if (   a == 1
            && b == 0
            && c == 0
            && d == 1
            && e == 0
            && f == 0)
            return QString();

        transform = TransformType::Matrix;
        newPoints.reserve(6);
        newPoints << fromDouble(a, Round::Transform);
        newPoints << fromDouble(b, Round::Transform);
        newPoints << fromDouble(c, Round::Transform);
        newPoints << fromDouble(d, Round::Transform);
        newPoints << fromDouble(e, Round::Coordinate);
        newPoints << fromDouble(f, Round::Coordinate);
    }

    transform += QL1S("(");
    bool isTrim = Keys::get().flag(Key::RemoveUnneededSymbols);
    for (int i = 0; i < newPoints.size(); ++i) {
        if (i != 0) {
            if (isTrim) {
                if ((!newPoints.at(i-1).contains(QL1C('.'))
                     && newPoints.at(i).startsWith(QL1C('.')))
                        || newPoints.at(i).at(0).isDigit())
                    transform += QL1S(" ");
            } else {
                transform +=QL1S(" ");
            }
        }
        transform += newPoints.at(i);
    }
    transform += QL1S(")");
    return transform;
}

void TransformPrivate::divide(const QString &text)
{
    if (text.isEmpty() || text.count(QL1C(' ')) == text.size()) {
        divide(QL1S("translate(0,0)"));
        return;
    }
    QList<TransformMatrix> transMatrixList = parseTransform(text);

    TransformMatrix newMatrix = transMatrixList.at(0);
    for (int i = 1; i < transMatrixList.count(); ++i)
        newMatrix = newMatrix * transMatrixList.at(i);
    newMatrix.invert();
    newMatrix = matrix() * newMatrix;
    calcParameters(newMatrix);
}

TransformMatrix TransformPrivate::matrix() const
{
    return TransformMatrix(a, b, c, d, e, f);
}

QList<TransformMatrix> TransformPrivate::parseTransform(const QString &text)
{
    return parseTransform(text.constData(), text.size());
}

QList<TransformMatrix> TransformPrivate::parseTransform(const QChar *str, int size)
{
    QList<TransformMatrix> list;
    StringWalker sw(str, size);
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
            qFatal("wrong transform matrix: %s", qPrintable(QString(str, size)));
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

void TransformPrivate::calcMatrixes(const QString &text)
{
    calcMatrixes(text.constData(), text.size());
}

void TransformPrivate::calcMatrixes(const QChar *str, int size)
{
    QList<TransformMatrix> transMatrixList;
    if (size == 0)
        transMatrixList << TransformMatrix();
    else
        transMatrixList = parseTransform(str, size);

    TransformMatrix newMatrix = transMatrixList.at(0);
    for (int i = 1; i < transMatrixList.count(); ++i)
        newMatrix = newMatrix * transMatrixList.at(i);
    calcParameters(newMatrix);
}

void TransformPrivate::calcParameters(const TransformMatrix &matrix)
{
    isChanged = true;

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

    xScale = sqrt(a*a + c*c);
    yScale = sqrt(b*b + d*d);
    xSkew = atan(b)*(180.0/M_PI);
    ySkew = atan(c)*(180.0/M_PI);
    angle = atan(-b/a)*(180/M_PI);
    if (qIsNaN(angle))
        qFatal("rotation is NaN");

    if (b < c)
        angle = -angle;

//    qDebug() << "calc" << xScale << yScale << xSkew << ySkew << angle;

    // detect all the transformations inside the matrix

    types = Types();

    if (f != 0 || e != 0)
        types |= Translate;

    if ((isZeroTs(qAbs(b) - qAbs(c)) && b != 0) || !isZero(angle))
        types |= Rotate;

    if (xScale != 1 || yScale != 1)
        types |= Scale;

    if (isZeroTs(xScale - yScale))
        types |= ProportionalScale;

    if (xSkew != 0 || ySkew != 0)
        types |= Skew;

    if (xScale < 0 || a < 0)
        types |= HorizontalMirror;

    if (yScale < 0 || d < 0)
        types |= VertiacalMirror;
}

Transform::Transform(const QString &text)
{
    impl = new TransformPrivate();
    if (text.isEmpty())
        impl->calcMatrixes(QString());
    else
        impl->calcMatrixes(text);
}

Transform::Transform(const QChar *str, int size)
{
    impl = new TransformPrivate();
    if (size == 0)
        impl->calcMatrixes(QString());
    else
        impl->calcMatrixes(str, size);
}

Transform::Transform(const Transform &t) : impl(t.impl) {}

bool Transform::operator!=(const Transform &t) const
{
    return !(*this == t);
}

bool Transform::operator==(const Transform &t) const
{
    // compare pointers
    if (impl == t.impl)
        return true;

    // compare data
    if (   impl && t.impl
        && isZero(impl->e - t.impl->e)
        && isZero(impl->f - t.impl->f)
        && isZeroTs(impl->a - t.impl->a)
        && isZeroTs(impl->d - t.impl->d)
        && isZeroTs(impl->b - t.impl->b)
        && isZeroTs(impl->c - t.impl->c))
        return true;

    return false;
}

bool Transform::isNull() const
{
    return !impl;
}

bool Transform::isValid() const
{
    if (!impl)
        return false;
    return !(   qFuzzyCompare(impl->a, 1.0)
             && isZeroTs(impl->b)
             && isZeroTs(impl->c)
             && qFuzzyCompare(impl->d, 1.0)
             && isZero(impl->e)
             && isZero(impl->f));
}

void Transform::clear()
{
    impl = 0;
}

void Transform::append(const Transform &ts)
{
    if (!impl)
        return;
    impl->calcParameters(matrix() * ts.matrix());
}

void Transform::setOldXY(double prevX, double prevY)
{
    if (!impl)
        return;
    impl->oldX = prevX;
    impl->oldY = prevY;
}

void Transform::divide(const QString &text)
{
    if (!impl)
        return;
    impl->divide(text);
}

QRectF Transform::transformRect(const QRectF &rect)
{
    if (!impl)
        return QRectF();

    // cannot apply skew transform to rect
    Q_ASSERT(isSkew() == false);

    double xList[4];
    double yList[4];

    setOldXY(rect.x(), rect.y());
    xList[0] = newX();
    yList[0] = newY();

    setOldXY(rect.x() + rect.width(), rect.y());
    xList[1] = newX();
    yList[1] = newY();

    setOldXY(rect.x(), rect.y() + rect.height());
    xList[2] = newX();
    yList[2] = newY();

    setOldXY(rect.x() + rect.width(), rect.y() + rect.height());
    xList[3] = newX();
    yList[3] = newY();

    double minx, miny, maxx, maxy;
    minx = maxx = xList[0];
    miny = maxy = yList[0];
    for (int i = 0; i < 4; ++i) {
        const double x = xList[i];
        maxx = qMax(x, maxx);
        minx = qMin(x, minx);

        const double y = yList[i];
        maxy = qMax(y, maxy);
        miny = qMin(y, miny);
    }
    return QRectF(minx, miny, maxx - minx, maxy - miny);
}

double Transform::newX() const
{
    if (!impl)
        return 0;
    return impl->a * impl->oldX + impl->c * impl->oldY + impl->e;
}

double Transform::newY() const
{
    if (!impl)
        return 0;
    return impl->b * impl->oldX + impl->d * impl->oldY + impl->f;
}

Transform::Transform(TransformPrivate *t)
{
    impl = t;
}

QString Transform::simplified()
{
    if (!impl)
        return QString();
    if (impl->isChanged) {
        impl->lastSimplified = impl->simplified();
        impl->isChanged = false;
    }
    return impl->lastSimplified;
}

QString Transform::simplified() const
{
    if (!impl)
        return QString();
    return impl->simplified();
}

double Transform::scaleFactor() const
{
    if (!impl)
        return 0;
    return impl->xScale;
}

bool Transform::isProportionalScale() const
{
    if (!impl)
        return false;
    return impl->types.testFlag(TransformPrivate::ProportionalScale);
}

bool Transform::isMirrored() const
{
    if (!impl)
        return false;
    return    impl->types.testFlag(TransformPrivate::VertiacalMirror)
           || impl->types.testFlag(TransformPrivate::HorizontalMirror);
}

bool Transform::isSkew() const
{
    if (!impl)
        return false;
    return impl->types.testFlag(TransformPrivate::Skew);
}

bool Transform::isRotating() const
{
    if (!impl)
        return false;
    return impl->types.testFlag(TransformPrivate::Rotate);
}

bool Transform::isTranslate() const
{
    if (!impl)
        return false;
    return impl->types.testFlag(TransformPrivate::Translate);
}

TransformMatrix Transform::matrix() const
{
    if (!impl)
        return TransformMatrix();
    return impl->matrix();
}

QString Transform::matrixString() const
{
    return QString(QL1S("matrix:\n"
            "( %1, %3, %5 )\n"
            "( %2, %4, %6 )")).arg(impl->a).arg(impl->b).arg(impl->c)
                              .arg(impl->d).arg(impl->e).arg(impl->f);
}

TransformPrivate::Types Transform::type() const
{
    if (!impl)
        return TransformPrivate::Types();
    return impl->types;
}

QDebug operator<<(QDebug dbg, const Transform &t)
{
    TransformMatrix m = t.matrix();
    dbg << "Transform (" << m(0,0) << m(1,0) << m(0,1) << m(1,1) << m(0,2) << m(1,2) << ")";
    if (t.isProportionalScale())
        dbg << "ProportionalScale";
    if (t.isMirrored())
        dbg << "Mirrored";
    if (t.isSkew())
        dbg << "Skew";
    if (t.isRotating())
        dbg << "Rotating";
    return dbg.space();
}
