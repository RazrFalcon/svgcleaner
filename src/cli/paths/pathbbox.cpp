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

#include <QPointF>

#include <cmath>

#include "../mindef.h"
#include "../tools.h"
#include "pathbbox.h"

// path bounding box calculation

struct Box
{
    double minx;
    double miny;
    double maxx;
    double maxy;
};

struct Bezier
{
    double p_x;
    double p_y;
    double x1;
    double y1;
    double x2;
    double y2;
    double x;
    double y;
};

void updateBBox(Box &bbox, double x, double y)
{
    bbox.maxx = qMax(x, bbox.maxx);
    bbox.minx = qMin(x, bbox.minx);
    bbox.maxy = qMax(y, bbox.maxy);
    bbox.miny = qMin(y, bbox.miny);
}

QPointF findDotOnCurve(const Bezier &b, double t)
{
    double t1 = 1 - t;
    double t_2 = t * t;
    double t_3 = t_2 * t;
    double t1_2 = t1 * t1;
    double t1_3 = t1_2 * t1;
    double x =  t1_3 * b.p_x
              + t1_2 * 3 * t * b.x1
              + t1 * 3 * t_2 * b.x2
              + t_3 * b.x;
    double y =  t1_3 * b.p_y
              + t1_2 * 3 * t * b.y1
              + t1 * 3 * t_2 * b.y2
              + t_3 * b.y;
    return QPointF(x, y);
}

void curveExtremum(double a, double b, double c, const Bezier &bezier, Box &bbox)
{
    double t1 = (-b + sqrt(b * b - 4 * a * c)) / 2.0 / a;
    double t2 = (-b - sqrt(b * b - 4 * a * c)) / 2.0 / a;

    if (t1 >= 0.0 && t1 <= 1.0) {
        QPointF p = findDotOnCurve(bezier, t1);
        updateBBox(bbox, p.x(), p.y());
    }
    if (t2 >= 0.0 && t2 <= 1.0) {
        QPointF p = findDotOnCurve(bezier, t2);
        updateBBox(bbox, p.x(), p.y());
    }
}

void curveBBox(const Bezier &bz, Box &bbox)
{
    updateBBox(bbox, bz.p_x, bz.p_y);
    updateBBox(bbox, bz.x, bz.y);

    double a = (bz.x2 - 2 * bz.x1 + bz.p_x) - (bz.x - 2 * bz.x2 + bz.x1);
    double b = 2 * (bz.x1 - bz.p_x) - 2 * (bz.x2 - bz.x1);
    double c = bz.p_x - bz.x1;
    curveExtremum(a, b, c, bz, bbox);

    a = (bz.y2 - 2 * bz.y1 + bz.p_y) - (bz.y - 2 * bz.y2 + bz.y1);
    b = 2 * (bz.y1 - bz.p_y) - 2 * (bz.y2 - bz.y1);
    c = bz.p_y - bz.y1;
    curveExtremum(a, b, c, bz, bbox);
}

// TODO: stroke are ignored
QString PathBBox::calcBoundingBox(const PathSegmentList &segList, bool convertToCurves)
{
    // convert all segments to curve, exept m and z
    PathSegmentList tmpSegList = segList;
    if (convertToCurves) {
        PathSegment prevSegment2;
        for (int i = 1; i < tmpSegList.size(); ++i) {
            PathSegment prevSegment = tmpSegList.at(i-1);
            PathSegment currSegment = tmpSegList.at(i);
            QVarLengthArray<PathSegment, 3> list = tmpSegList.at(i).toCurve(prevSegment,
                                                                            prevSegment2);
            prevSegment2 = currSegment;
            if (!list.isEmpty()) {
                tmpSegList.removeAt(i);
                for (int j = 0; j < list.count(); ++j)
                    tmpSegList.insert(i+j, list.at(j));
            }
        }
    }
    // get all points
    Box bbox = { tmpSegList.at(0).x, tmpSegList.at(0).y,
                 tmpSegList.at(0).x, tmpSegList.at(0).y };
    for (int i = 0; i < tmpSegList.size(); ++i) {
        PathSegment seg = tmpSegList.at(i);
        if (seg.command == Command::MoveTo) {
            updateBBox(bbox, seg.x, seg.y);
        } else if (seg.command == Command::CurveTo) {
            Bezier bezier = {tmpSegList.at(i-1).x, tmpSegList.at(i-1).y,
                             seg.x1, seg.y1, seg.x2, seg.y2, seg.x, seg.y};
            curveBBox(bezier, bbox);
        }
    }

    return                fromDouble(bbox.minx)
            + QL1C(' ') + fromDouble(bbox.miny)
            + QL1C(' ') + fromDouble(bbox.maxx - bbox.minx)
            + QL1C(' ') + fromDouble(bbox.maxy - bbox.miny);
}
