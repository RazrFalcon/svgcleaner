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

#ifndef PATHS_H
#define PATHS_H

#include <QVarLengthArray>

#include "transform.h"
#include "svgdom.h"

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

class Segment
{
public:
    Segment();
    void setTransform(Transform &ts);
    void toRelative(double xLast, double yLast);
    void coords(QVarLengthArray<double, 6> &points);
    QVarLengthArray<Segment, 3> toCurve(double prevX, double prevY) const;

    QChar command;
    // store original value
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

class Path
{
public:
    explicit Path() {}
    void processPath(SvgElement elem, bool canApplyTransform, bool *isPathApplyed);
    QString segmentsToPath(const QList<Segment> &segList);

private:
    SvgElement m_elem;

    void calcBoundingBox(const QList<Segment> &segList);
    void splitToSegments(const QString &path, QList<Segment> &segList);
    void processSegments(QList<Segment> &segList);
    void segmentsToRelative(QList<Segment> &segList, bool onlyIfSourceWasRelative);
    void calcNewStrokeWidth(const double scaleFactor);
    bool applyTransform(QList<Segment> &segList);
    bool isTsPathShorter();
    void fixRelative(QList<Segment> &segList);

    // segments processing
    bool removeZSegments(QList<Segment> &segList);
    bool removeUnneededMoveToSegments(QList<Segment> &segList);
    bool removeTinySegments(QList<Segment> &segList);
    bool convertSegments(QList<Segment> &segList);
    bool joinSegments(QList<Segment> &segList);
    void removeStrokeLinecap(QList<Segment> &segList);
};

#endif // PATHS_H
