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

#ifndef PATHS_H
#define PATHS_H

#include <QVector>

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
    qreal x1;
    qreal y1;
    qreal rx;
    qreal ry;
    qreal angle;
    bool large_arc_flag;
    bool sweep_flag;
    qreal x2;
    qreal y2;
    QList<qreal> recursive;
};

class Segment
{
public:
    Segment();
    void setTransform(Transform &ts);
    void toRelative(qreal xLast, qreal yLast);
    void coords(QVector<qreal> &points);
    QList<Segment> toCurve(qreal prevX, qreal prevY) const;

    QChar command;
    // store original value
    bool absolute;
    // is this command defined in source path
    bool srcCmd;
    qreal x;
    qreal y;
    qreal x1;
    qreal y1;
    qreal x2;
    qreal y2;
    qreal rx;
    qreal ry;
    qreal xAxisRotation;
    bool largeArc;
    bool sweep;

private:
    QPointF rotatePoint(qreal x, qreal y, qreal rad) const;
    QList<QPointF> arcToCurve(ArcStruct arc) const;
};

class Path
{
public:
    explicit Path() {}
    void processPath(SvgElement elem, bool canApplyTransform, bool *isPathApplyed);
    QString segmentsToPath(QList<Segment> &segList);

private:
    SvgElement m_elem;

    void calcBoundingBox(const QList<Segment> &segList);
    void splitToSegments(const QString &path, QList<Segment> &segList);
    void processSegments(QList<Segment> &segList);
    void segmentsToRelative(QList<Segment> &segList, bool onlyIfSourceWasRelative);
    void calcNewStrokeWidth(const Transform &transform);
    bool applyTransform(QList<Segment> &segList);
    bool isTsPathShorter();
    void fixRelative(QList<Segment> &segList);
    QString findAttribute(const int &attrId);

    // segments processing
    bool removeZSegments(QList<Segment> &segList);
    bool removeUnneededMoveToSegments(QList<Segment> &segList);
    bool removeTinySegments(QList<Segment> &segList);
    bool convertSegments(QList<Segment> &segList);
    bool joinSegments(QList<Segment> &segList);
    void removeStrokeLinecap(QList<Segment> &segList);
};

#endif // PATHS_H
