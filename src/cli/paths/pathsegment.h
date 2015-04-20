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

#ifndef PATHSEGMENT_H
#define PATHSEGMENT_H

#include <QVarLengthArray>

#include "../transform.h"

namespace Command {
    extern const QChar MoveTo;
    extern const QChar LineTo;
    extern const QChar HorizontalLineTo;
    extern const QChar VerticalLineTo;
    extern const QChar CurveTo;
    extern const QChar SmoothCurveTo;
    extern const QChar Quadratic;
    extern const QChar SmoothQuadratic;
    extern const QChar EllipticalArc;
    extern const QChar ClosePath;
}

struct ArcStruct
{
    double x1;
    double y1;
    double rx;
    double ry;
    double angle;
    bool large_arc_flag;
    bool sweep_flag;
    double x2;
    double y2;
    QList<double> recursive;
};

class PathSegment
{
public:
    PathSegment();
    bool operator== (const PathSegment &s) const;
    bool operator!= (const PathSegment &s) const;

    void setTransform(Transform &ts);
    void toRelative(double xLast, double yLast);
    void coords(QVarLengthArray<double, 6> &points);
    QVarLengthArray<PathSegment, 3> toCurve(const PathSegment &prevSeg, const PathSegment &prevSeg2) const;

    QChar command;
    // stores original value
    bool absolute;
    // is this command defined in source path
    bool srcCmd;
    double x;
    double y;
    double x1;
    double y1;
    double x2;
    double y2;
    double rx;
    double ry;
    double xAxisRotation;
    bool largeArc;
    bool sweep;

private:
    QPointF rotatePoint(double x, double y, double rad) const;
    QVarLengthArray<QPointF, 9> arcToCurve(ArcStruct arc) const;
};

typedef QList<PathSegment> PathSegmentList;

QDebug operator<<(QDebug dbg, const PathSegment &s);

#endif // PATHSEGMENT_H
