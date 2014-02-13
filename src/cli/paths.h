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

#include "svgelement.h"

namespace Command {
    static const QChar MoveTo           = 'm';
    static const QChar LineTo           = 'l';
    static const QChar HorizontalLineTo = 'h';
    static const QChar VerticalLineTo   = 'v';
    static const QChar CurveTo          = 'c';
    static const QChar SmoothCurveTo    = 's';
    static const QChar Quadratic        = 'q';
    static const QChar SmoothQuadratic  = 't';
    static const QChar EllipticalArc    = 'a';
    static const QChar ClosePath        = 'z';
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
    void splitToSegments(const QStringRef &path, QList<Segment> &segList);
    void processSegments(QList<Segment> &segList);
    void segmentsToRelative(QList<Segment> &segList, bool onlyIfSourceWasRelative);
    bool isZero(double value);
    void calcNewStrokeWidth(const Transform &transform);
    bool applyTransform(QList<Segment> &segList);
    bool isTsPathShorter();
    void fixRelative(QList<Segment> &segList);
    QString findAttribute(const QString &attrName);
};

#endif // PATHS_H
