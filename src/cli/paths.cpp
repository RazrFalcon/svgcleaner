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

#include "tools.h"
#include "paths.h"

#define Keys Keys::get()

using namespace Attribute;
using namespace DefaultValue;

// http://www.w3.org/TR/SVG/paths.html

// TODO: add path approximation
//       fjudy2001_02.svg
//       warszawianka_Betel.svg

namespace Command {
    const QChar MoveTo           = QL1C('m');
    const QChar LineTo           = QL1C('l');
    const QChar HorizontalLineTo = QL1C('h');
    const QChar VerticalLineTo   = QL1C('v');
    const QChar CurveTo          = QL1C('c');
    const QChar SmoothCurveTo    = QL1C('s');
    const QChar Quadratic        = QL1C('q');
    const QChar SmoothQuadratic  = QL1C('t');
    const QChar EllipticalArc    = QL1C('a');
    const QChar ClosePath        = QL1C('z');
}

Segment::Segment()
{
    absolute = false;
    srcCmd   = false;
    largeArc = false;
    sweep    = false;
}

void Segment::setTransform(Transform &ts)
{
    if (   command == Command::MoveTo
        || command == Command::LineTo
        || command == Command::SmoothQuadratic) {
        ts.setOldXY(x, y);
        x = ts.newX();
        y = ts.newY();
    } else if (command == Command::CurveTo) {
        ts.setOldXY(x, y);
        x = ts.newX();
        y = ts.newY();

        ts.setOldXY(x1, y1);
        x1 = ts.newX();
        y1 = ts.newY();

        ts.setOldXY(x2, y2);
        x2 = ts.newX();
        y2 = ts.newY();
    } else if (command == Command::SmoothCurveTo) {
        ts.setOldXY(x, y);
        x = ts.newX();
        y = ts.newY();

        ts.setOldXY(x2, y2);
        x2 = ts.newX();
        y2 = ts.newY();
    } else if (command == Command::Quadratic) {
        ts.setOldXY(x, y);
        x = ts.newX();
        y = ts.newY();

        ts.setOldXY(x1, y1);
        x1 = ts.newX();
        y1 = ts.newY();
    }
}

QPointF Segment::rotatePoint(double x, double y, double rad) const
{
    double X = x * cos(rad) - y * sin(rad);
    double Y = x * sin(rad) + y * cos(rad);
    return QPointF(X, Y);
}

// arcToCurve is a port from Raphael JavaScript library to Qt (MIT license)
QVarLengthArray<QPointF, 9> Segment::arcToCurve(ArcStruct arc) const
{
    double f1 = 0;
    double f2 = 0;
    double cx = 0;
    double cy = 0;
    double rad = M_PI / 180 * arc.angle;
    if (arc.recursive.isEmpty()) {
        QPointF p = rotatePoint(arc.x1, arc.y1, -rad);
        arc.x1 = p.x();
        arc.y1 = p.y();
        p = rotatePoint(arc.x2, arc.y2, -rad);
        arc.x2 = p.x();
        arc.y2 = p.y();

        double x = (arc.x1 - arc.x2) / 2;
        double y = (arc.y1 - arc.y2) / 2;
        double h = (x * x) / (arc.rx * arc.rx) + (y * y) / (arc.ry * arc.ry);
        if (h > 1) {
            h = sqrt(h);
            arc.rx = h * arc.rx;
            arc.ry = h * arc.ry;
        }

        double rx2 = arc.rx * arc.rx;
        double ry2 = arc.ry * arc.ry;
        double kk = 1;
        if (arc.large_arc_flag == arc.sweep_flag)
            kk = -1;
        double k = kk * sqrt(fabs((rx2 * ry2 - rx2 * y * y - ry2 * x * x)
                                     / (rx2 * y * y + ry2 * x * x)));
        cx = k * arc.rx * y / arc.ry + (arc.x1 + arc.x2) / 2;
        cy = k * -arc.ry * x / arc.rx + (arc.y1 + arc.y2) / 2;

        double tt = (arc.y1 - cy) / arc.ry;
        if (tt > 1)
            tt = 1;
        if (tt < -1)
            tt = -1;
        f1 = asin(tt);

        double tt2 = (arc.y2 - cy) / arc.ry;
        if (tt2 > 1)
            tt2 = 1;
        if (tt2 < -1)
            tt2 = -1;
        f2 = asin(tt2);

        if (arc.x1 < cx)
            f1 = M_PI - f1;
        if (arc.x2 < cx)
            f2 = M_PI - f2;

        if (f1 < 0)
            f1 = M_PI * 2 + f1;
        if (f2 < 0)
            f2 = M_PI * 2 + f2;
        if (arc.sweep_flag && f1 > f2)
            f1 = f1 - M_PI * 2;
        if (!arc.sweep_flag && f2 > f1)
            f2 = f2 - M_PI * 2;
    } else {
        f1 = arc.recursive.at(0);
        f2 = arc.recursive.at(1);
        cx = arc.recursive.at(2);
        cy = arc.recursive.at(3);
    }

    QVarLengthArray<QPointF, 9> res;
    double df = f2 - f1;
    const double a90 = M_PI * 90 / 180;
    if (std::abs(df) > a90) {
        double f2old = f2;
        double x2old = arc.x2;
        double y2old = arc.y2;
        double c = -1;
        if (arc.sweep_flag && f2 > f1)
            c = 1;
        f2 = f1 + a90 * c;
        arc.x2 = cx + arc.rx * cos(f2);
        arc.y2 = cy + arc.ry * sin(f2);
        QList<double> list;
        list.reserve(4);
        list << f2 << f2old << cx << cy;
        ArcStruct newArk = { arc.x2, arc.y2, arc.rx, arc.ry, arc.angle, 0,
                             arc.sweep_flag, x2old, y2old, list };
        res = arcToCurve(newArk);
    }

    df = f2 - f1;
    double c1 = cos(f1);
    double s1 = sin(f1);
    double c2 = cos(f2);
    double s2 = sin(f2);
    double t = tan(df / 4.0);
    double hx = 4.0 / 3.0 * arc.rx * t;
    double hy = 4.0 / 3.0 * arc.ry * t;

    QPointF p1(arc.x1, arc.y1);
    QPointF p2(arc.x1 + hx * s1, arc.y1 - hy * c1);
    QPointF p3(arc.x2 + hx * s2, arc.y2 - hy * c2);
    QPointF p4(arc.x2, arc.y2);

    p2.setX(2 * p1.x() - p2.x());
    p2.setY(2 * p1.y() - p2.y());

    QVarLengthArray<QPointF, 9> list;
    list.reserve(4);
    list << p2 << p3 << p4;
    for (int i = 0; i < res.size(); ++i)
        list << res.at(i);
    return list;
}

Segment quadToCurve(double x0, double y0, double x1, double y1, double x2, double y2)
{
    static const double v1 = 1.0 / 3.0;
    static const double v2 = 2.0 / 3.0;

    Segment seg;
    seg.command = Command::CurveTo;
    seg.absolute = true;
    seg.x1 = x0 + (x1-x0) * v2;
    seg.y1 = y0 + (y1-y0) * v2;
    seg.x2 = x1 + (x2-x1) * v1;
    seg.y2 = y1 + (y2-y1) * v1;
    seg.x  = x2;
    seg.y  = y2;
    return seg;
}

Segment lineToCurve(double x1, double y1, double x2, double y2)
{
    Segment seg;
    seg.command = Command::CurveTo;
    seg.absolute = true;
    seg.x1 = x1;
    seg.y1 = y1;
    seg.x2 = x2;
    seg.y2 = y2;
    seg.x  = x2;
    seg.y  = y2;
    return seg;
}

QVarLengthArray<Segment, 3> Segment::toCurve(const Segment &prevSeg, const Segment &prevSeg2) const
{
    QVarLengthArray<Segment, 3> segList;
    if (command == Command::HorizontalLineTo)
    {
        segList.append(lineToCurve(prevSeg.x, prevSeg.y, x, prevSeg.y));
    }
    else if (command == Command::VerticalLineTo)
    {
        segList.append(lineToCurve(prevSeg.x, prevSeg.y, prevSeg.x, y));
    }
    else if (command == Command::LineTo)
    {
        segList.append(lineToCurve(prevSeg.x, prevSeg.y, x, y));
    }
    else if (command == Command::SmoothCurveTo)
    {
        Segment seg;
        seg.command = Command::CurveTo;
        seg.absolute = true;
        if (prevSeg.command == Command::CurveTo || prevSeg.command == Command::SmoothCurveTo) {
            seg.x1 = prevSeg.x * 2 - prevSeg.x2;
            seg.y1 = prevSeg.y * 2 - prevSeg.y2;
        } else {
            seg.x1 = prevSeg.x;
            seg.y1 = prevSeg.y;
        }
        seg.x2 = x2;
        seg.y2 = y2;
        seg.x  = x;
        seg.y  = y;
        segList.append(seg);
    }
    else if (command == Command::Quadratic)
    {
        segList.append(quadToCurve(prevSeg.x, prevSeg.y, x1, y1, x, y));
    }
    else if (command == Command::SmoothQuadratic)
    {
        Segment seg = prevSeg2;
        if (prevSeg2.command == Command::Quadratic || prevSeg2.command == Command::SmoothQuadratic) {
            seg.x1 = prevSeg2.x * 2 - prevSeg2.x1;
            seg.y1 = prevSeg2.y * 2 - prevSeg2.y1;
        } else {
            seg.x1 = prevSeg2.x;
            seg.y1 = prevSeg2.y;
        }

        segList.append(quadToCurve(prevSeg2.x1, prevSeg2.y1, seg.x1, seg.y1, x, y));
    }
    else if (command == Command::EllipticalArc)
    {
        if (!isZero(rx) && !isZero(ry)) {
            ArcStruct arc = { prevSeg.x, prevSeg.y, rx, ry, xAxisRotation, largeArc,
                              sweep, x, y, QList<double>() };
            QVarLengthArray<QPointF, 9> list = arcToCurve(arc);
            segList.reserve(list.size()/3);
            for (int i = 0; i < list.size(); i += 3) {
                Segment seg;
                seg.command = Command::CurveTo;
                seg.absolute = true;
                seg.x1 = list.at(i).x();
                seg.y1 = list.at(i).y();
                seg.x2 = list.at(i+1).x();
                seg.y2 = list.at(i+1).y();
                seg.x  = list.at(i+2).x();
                seg.y  = list.at(i+2).y();
                segList.append(seg);
            }
        }
    }
    return segList;
}

void Segment::toRelative(double xLast, double yLast)
{
    x -= xLast;
    y -= yLast;
    if (command == Command::CurveTo) {
        x1 -= xLast;
        y1 -= yLast;
        x2 -= xLast;
        y2 -= yLast;
    } else if (command == Command::SmoothCurveTo) {
        x2 -= xLast;
        y2 -= yLast;
    } else if (command == Command::Quadratic) {
        x1 -= xLast;
        y1 -= yLast;
    }
    absolute = false;
}

void Segment::coords(QVarLengthArray<double, 6> &points)
{
    if (command == Command::CurveTo)
        points << x1 << y1 << x2 << y2 << x << y;
    else if (  command == Command::MoveTo
            || command == Command::LineTo
            || command == Command::SmoothQuadratic)
        points << x << y;
    else if (command == Command::HorizontalLineTo)
        points << x;
    else if (command == Command::VerticalLineTo)
        points << y;
    else if (command == Command::SmoothCurveTo)
        points << x2 << y2 << x << y;
    else if (command == Command::Quadratic)
        points << x1 << y1 << x << y;
    else if (command == Command::EllipticalArc)
        points << rx << ry << xAxisRotation << largeArc << sweep << x << y;
}

QDebug operator<<(QDebug dbg, const Segment &s)
{
    if (s.command == Command::CurveTo)
        dbg << QString(QL1S("CurveTo( %1,%2 - %3,%4 - %5,%6 )"))
                   .arg(s.x1).arg(s.y1).arg(s.x2).arg(s.y2).arg(s.x).arg(s.y);
    else if (s.command == Command::MoveTo)
        dbg << "MoveTo(" << s.x << s.y << ")";
    else if (s.command == Command::LineTo)
        dbg << "LineTo(" << s.x << s.y << ")";
    else if (s.command == Command::SmoothQuadratic)
        dbg << "SmoothQuadratic(" << s.x << s.y << ")";
    else if (s.command == Command::HorizontalLineTo)
        dbg << "HorizontalLineTo(" << s.x << ")";
    else if (s.command == Command::VerticalLineTo)
        dbg << "VerticalLineTo(" << s.y << ")";
    else if (s.command == Command::SmoothCurveTo)
        dbg << "SmoothCurveTo(" << s.x2 << s.y2 << s.x << s.y << ")";
    else if (s.command == Command::Quadratic)
        dbg << "Quadratic(" << s.x1 << s.y1 << s.x << s.y << ")";
    else if (s.command == Command::EllipticalArc)
        dbg << "EllipticalArc(" << s.rx << s.ry << s.xAxisRotation
            << s.largeArc << s.sweep << s.x << s.y << ")";

    return dbg.space();
}

// New class

void Path::processPath(SvgElement elem, bool canApplyTransform, bool *isTransformApplied)
{
    const QString inPath = elem.attribute(AttrId::d);

    if (inPath.contains(QL1S("nan"))) {
        elem.setAttribute(AttrId::d, QString());
        return;
    }
    m_elem = elem;

    QList<Segment> segList;
    splitToSegments(inPath, segList);
    // paths without segments or with first segment not 'moveto' are invalid
    if (segList.isEmpty() || segList.first().command != Command::MoveTo) {
        if (Keys.flag(Key::RemoveInvisibleElements))
            elem.setAttribute(AttrId::d, QString());
        return;
    }

    // path like 'M x,y A rx,ry 0 1 1 x,y', where x and y of both segments are similar
    // can't be converted to relative coordinates
    // TODO: MA could be in subpath
    bool isOnlyAbsolute = false;
    if (segList.at(1).command == Command::EllipticalArc) {
        isOnlyAbsolute = true;
        canApplyTransform = false;
    }

    processSegments(segList);

    QList<Segment> tsSegList = segList;

    if (Keys.flag(Key::RemoveOutsideElements))
        elem.setAttribute(AttrId::bbox, calcBoundingBox(segList, true));

    const bool convToRelative = !(!isOnlyAbsolute && Keys.flag(Key::ConvertToRelative));
    segmentsToRelative(segList, convToRelative);

    // merge segments to path
    QString outPath = segmentsToPath(segList);

    // set old path, if new is longer
    if (outPath.size() > inPath.size())
        outPath = inPath;
    elem.setAttribute(AttrId::d, outPath);

    // try to bake transforms into path
    if (canApplyTransform) {
        applyTransform(tsSegList);

        if (Keys.flag(Key::RemoveOutsideElements))
            elem.setAttribute(QL1S("bboxts"), calcBoundingBox(tsSegList, false));

        processSegments(tsSegList);
        if (Keys.flag(Key::ConvertToRelative))
            segmentsToRelative(tsSegList, false);
        calcNewStrokeWidth(m_elem.transform().scaleFactor());
        m_elem.setAttribute(QL1S("dts"), segmentsToPath(tsSegList));

        // apply transform only if it shorter
        if (isTsPathShorter()) {
            *isTransformApplied = true;
            elem.setAttribute(AttrId::d, elem.attribute(QL1S("dts")));
            QString newStroke = elem.attribute(QL1S("stroke-width-new"));
            if (!newStroke.isEmpty() && newStroke != QL1S("1"))
                elem.setAttribute(AttrId::stroke_width, newStroke);
            else
                elem.removeAttribute(AttrId::stroke_width);
            elem.setAttribute(AttrId::bbox, elem.attribute(QL1S("bboxts")));
        }
        elem.removeAttribute(QL1S("stroke-width-new"));
        elem.removeAttribute(QL1S("dts"));
        elem.removeAttribute(QL1S("bboxts"));
    }
}

bool isUpper(const QChar &c)
{
    if (c.unicode() >= 65 && c.unicode() <= 90)
        return true;
    return false;
}

bool isLetter(const QChar &c)
{
    if (c.unicode() >= 65 && c.unicode() <= 90)
        return true;
    if (c.unicode() >= 97 && c.unicode() <= 122)
        return true;
    return false;
}

QChar toLower(const QChar &c)
{
    switch (c.unicode()) {
        case 65 : return QL1C('a'); break;
        case 67 : return QL1C('c'); break;
        case 72 : return QL1C('h'); break;
        case 76 : return QL1C('l'); break;
        case 77 : return QL1C('m'); break;
        case 81 : return QL1C('q'); break;
        case 83 : return QL1C('s'); break;
        case 84 : return QL1C('t'); break;
        case 86 : return QL1C('v'); break;
        case 90 : return QL1C('z'); break;
    default: break;
    }
    return c;
}

// split path to segments and convert segments to absolute
void Path::splitToSegments(const QString &path, QList<Segment> &segList)
{
    StringWalker sw(path);
    QChar cmd;
    QChar prevCmd;
    bool isNewCmd = false;
    double prevX = 0;
    double prevY = 0;
    double prevMX = 0;
    double prevMY = 0;
    while (sw.isValid() && !sw.atEnd()) {
        sw.skipSpaces();
        const QChar currChar = sw.current();
        if (isLetter(currChar)) {
            isNewCmd = true;
            if (toLower(currChar) == Command::ClosePath) {
                Segment seg;
                seg.command = Command::ClosePath;
                seg.absolute = false;
                seg.srcCmd = true;
                segList << seg;
                prevX = prevMX;
                prevY = prevMY;
            }
            prevCmd = toLower(cmd);
            cmd = currChar;
            sw.next();
        } else if (cmd != Command::ClosePath) {
            double offsetX = 0;
            double offsetY = 0;
            Segment seg;
            seg.command = toLower(cmd);
            if (seg.command != Command::ClosePath)
                seg.absolute = isUpper(cmd);

            if (!seg.absolute) {
                offsetX = prevX;
                offsetY = prevY;
            }
            seg.srcCmd = isNewCmd;
            isNewCmd = false;
            if (seg.command == Command::MoveTo) {
                if (seg.srcCmd) {
                    if (seg.absolute) {
                        offsetX = 0;
                        offsetY = 0;
                    } else if (prevCmd == Command::ClosePath) {
                        offsetX = prevMX;
                        offsetY = prevMY;
                    }
                }
                if (!seg.srcCmd)
                    seg.command = Command::LineTo;
                seg.x = sw.number() + offsetX;
                seg.y = sw.number() + offsetY;
                if (seg.srcCmd) {
                    prevMX = seg.x;
                    prevMY = seg.y;
                }
            } else if (   seg.command == Command::LineTo
                       || seg.command == Command::SmoothQuadratic) {
                seg.x = sw.number() + offsetX;
                seg.y = sw.number() + offsetY;
            } else if (seg.command == Command::HorizontalLineTo) {
                seg.command = Command::LineTo;
                seg.x = sw.number() + offsetX;
                seg.y = segList.last().y;
            } else if (seg.command == Command::VerticalLineTo) {
                seg.command = Command::LineTo;
                seg.x = segList.last().x;
                seg.y = sw.number() + offsetY;
            } else if (seg.command == Command::CurveTo) {
                seg.x1 = sw.number() + offsetX;
                seg.y1 = sw.number() + offsetY;
                seg.x2 = sw.number() + offsetX;
                seg.y2 = sw.number() + offsetY;
                seg.x  = sw.number() + offsetX;
                seg.y  = sw.number() + offsetY;
            } else if (seg.command == Command::SmoothCurveTo) {
                seg.x2 = sw.number() + offsetX;
                seg.y2 = sw.number() + offsetY;
                seg.x  = sw.number() + offsetX;
                seg.y  = sw.number() + offsetY;
            } else if (seg.command == Command::Quadratic) {
                seg.x1 = sw.number() + offsetX;
                seg.y1 = sw.number() + offsetY;
                seg.x  = sw.number() + offsetX;
                seg.y  = sw.number() + offsetY;
            } else if (seg.command == Command::EllipticalArc) {
                seg.rx = sw.number();
                seg.ry = sw.number();
                seg.xAxisRotation = sw.number();
                seg.largeArc      = sw.number();
                seg.sweep         = sw.number();
                seg.x = sw.number() + offsetX;
                seg.y = sw.number() + offsetY;
            } else {
                qFatal("wrong path format");
            }
            prevX = seg.x;
            prevY = seg.y;
            segList << seg;
            prevCmd = toLower(cmd);
        }
    }

    if (segList.size() == 1)
        segList.clear();
}

void Path::calcNewStrokeWidth(const double scaleFactor)
{
    SvgElement parentElem = m_elem;
    bool hasParentStrokeWidth = false;
    while (!parentElem.isNull()) {
        if (parentElem.hasAttribute(AttrId::stroke_width)) {
            double strokeWidth
                = toDouble(Tools::convertUnitsToPx(parentElem.attribute(AttrId::stroke_width)));
            QString sw = fromDouble(strokeWidth * scaleFactor, Round::Attribute);
            m_elem.setAttribute(QL1S("stroke-width-new"), sw);
            hasParentStrokeWidth = true;
            break;
        }
        parentElem = parentElem.parentElement();
    }
    if (!hasParentStrokeWidth) {
        m_elem.setAttribute(QL1S("stroke-width-new"), fromDouble(scaleFactor, Round::Attribute));
    }
}

void Path::applyTransform(QList<Segment> &tsSegList)
{
    Transform ts = m_elem.transform();

    Segment prevSegment2;
    for (int i = 1; i < tsSegList.count(); ++i) {
        Segment prevSegment = tsSegList.at(i-1);
        Segment currSeg = tsSegList.at(i);
        QVarLengthArray<Segment, 3> list = tsSegList.at(i).toCurve(prevSegment, prevSegment2);
        prevSegment2 = currSeg;
        if (!list.isEmpty()) {
            tsSegList.removeAt(i);
            for (int j = 0; j < list.count(); ++j)
                tsSegList.insert(i+j, list.at(j));
        }
    }
    for (int i = 0; i < tsSegList.count(); ++i)
        tsSegList[i].setTransform(ts);
}

bool Path::isTsPathShorter()
{
    static const int sts = QString(QL1S(" stroke-width=\"\" ")).size();

    quint32 afterTransform = m_elem.attribute(QL1S("dts")).size();
    if (m_elem.hasAttribute(QL1S("stroke-width-new"))) {
        afterTransform += m_elem.attribute(QL1S("stroke-width-new")).size();
        afterTransform += sts;
    }

    quint32 beforeTransform = m_elem.attribute(AttrId::d).size();
    if (m_elem.hasTransform()) {
        beforeTransform += m_elem.transform().simplified().size();
        static const int tss = QString(QL1S(" transform=\"\" ")).size();
        beforeTransform += tss;
    }
    if (m_elem.hasAttribute(AttrId::stroke_width)) {
        beforeTransform += m_elem.attribute(AttrId::stroke_width).size();
        beforeTransform += sts;
    }

    return (afterTransform < beforeTransform);
}

void Path::segmentsToRelative(QList<Segment> &segList, bool onlyIfSourceWasRelative)
{
    double lastX = 0;
    double lastY = 0;
    double lastMX = segList.first().x;
    double lastMY = segList.first().y;
    for (int i = 0; i < segList.count(); ++i) {
        Segment segment = segList.at(i);
        if (segment.command != Command::ClosePath) {
            if (onlyIfSourceWasRelative) {
                if (!segment.absolute)
                    segList[i].toRelative(lastX, lastY);
            } else {
                segList[i].toRelative(lastX, lastY);
            }
            lastX = segment.x;
            lastY = segment.y;
            if (i > 0 && segment.command == Command::MoveTo) {
                lastMX = lastX;
                lastMY = lastY;
            }
        } else {
            lastX = lastMX;
            lastY = lastMY;
        }
    }
}

bool isLineBasedSegment(const Segment &seg)
{
    return    seg.command == Command::LineTo
           || seg.command == Command::HorizontalLineTo
           || seg.command == Command::VerticalLineTo;
}

bool Path::removeZSegments(QList<Segment> &segList)
{
    if (segList.isEmpty())
        return false;

    // remove 'z' segment if last 'm' segment equal to segment before 'z'

    bool isRemoved = false;
    QString stroke = m_elem.parentAttribute(AttrId::stroke, true);
    bool hasStroke = (!stroke.isEmpty() && stroke != V_none);

    // FIXME: never used for some reason
    int lastMIdx = 0;
    QPointF prevM(segList.first().x, segList.first().y);
    for (int i = 1; i < segList.size(); ++i) {
        if (segList.at(i).command != Command::ClosePath)
            continue;

        Segment prevSeg = segList.at(i-1);
        if (isZero(prevM.x() - prevSeg.x) && isZero(prevM.y() - prevSeg.y)) {

            // Remove previous segment before 'z' segment if it's line like segment,
            // because 'z' actually will be replaced with the same line while rendering.
            // In path like: M 80 90 L 200 90 L 80 90 Z
            // segment 'L 80 90' is useless.
            if (isLineBasedSegment(prevSeg))
            {
                segList.removeAt(i-1);
                i--;
                isRemoved = true;
            } else if (!hasStroke) {
                segList.removeAt(i--);
                isRemoved = true;
            }
        }
        if (i != segList.size()-1) {
            prevM = QPointF(segList.at(i+1).x, segList.at(i+1).y);
            lastMIdx = i+1;
        }
    }

    if (   segList.size() == 3
        && isLineBasedSegment(segList.at(1))
        && segList.last().command == Command::ClosePath)
    {
        segList.removeLast();
        isRemoved = true;
    }

    return isRemoved;
}

bool Path::removeUnneededMoveToSegments(QList<Segment> &segList)
{
    if (segList.isEmpty())
        return false;

    // remove repeated MoveTo segments, if it was absolute in original path
    bool isRemoved = false;
    for (int i = 1; i < segList.count(); ++i) {
        Segment prevSeg = segList.at(i-1);
        Segment seg = segList.at(i);
        if (seg.command == Command::MoveTo) {
            if (prevSeg.command == Command::MoveTo && seg.absolute) {
                segList.removeAt(i-1);
                i--;
                isRemoved = true;
            }
        }
    }
    return isRemoved;
}

// BUG: do not remove segments at an acute angle
bool Path::removeTinySegments(QList<Segment> &segList)
{
    if (segList.isEmpty())
        return false;

    const int oldSize = segList.size();
    static double minValue = 1 / pow(10, Keys.coordinatesPrecision());

    // if current segment too close to previous - remove it
    for (int i = 0; i < segList.count()-1; ++i) {
        Segment seg = segList.at(i);
        Segment nextSeg = segList.at(i+1);
        if (   seg.command == nextSeg.command
            && seg.command != Command::ClosePath
            && seg.command != Command::EllipticalArc)
        {
            if (   qAbs(nextSeg.x - seg.x) < minValue
                && qAbs(nextSeg.y - seg.y) < minValue) {
                segList.removeAt(i+1);
                i--;
            }
        }
    }

    bool isElemHasFilter = !m_elem.parentAttribute(AttrId::filter, true).isEmpty();
    for (int i = 1; i < segList.count(); ++i) {
        Segment prevSeg = segList.at(i-1);
        Segment seg = segList.at(i);
        const QChar cmd = seg.command;
        const int tsize = segList.size();
        if (isZero(seg.x - prevSeg.x) && isZero(seg.y - prevSeg.y)) {
            if (cmd == Command::MoveTo) {
                // removing MoveTo from path with blur filter breaks render
                if (!isElemHasFilter && prevSeg.command == Command::MoveTo)
                    segList.removeAt(i--);
                else if (i == segList.size()-1 && prevSeg.command != Command::MoveTo)
                    segList.removeAt(i--);
            }
            else if (cmd == Command::CurveTo) {
                if (   isZero(seg.x1 - prevSeg.x1) && isZero(seg.y1 - prevSeg.y1)
                    && isZero(seg.x2 - prevSeg.x2) && isZero(seg.y2 - prevSeg.y2))
                {
                    segList.removeAt(i--);
                }
            }
            else if (cmd == Command::LineTo || cmd == Command::SmoothQuadratic) {
                segList.removeAt(i--);
            }
            else if (cmd == Command::SmoothCurveTo) {
                if (isZero(seg.x2 - prevSeg.x2) && isZero(seg.y2 - prevSeg.y2))
                    segList.removeAt(i--);
            }
            else if (cmd == Command::Quadratic) {
                if (isZero(seg.x1 - prevSeg.x1) && isZero(seg.y1 - prevSeg.y1))
                    segList.removeAt(i--);
            }
        }
        if (cmd == Command::ClosePath) {
            // remove paths like 'mz'
            if (i == segList.size()-1 && i == 1)
                segList.removeAt(i--);
        }
        if (tsize != segList.size()) {
            if (i < segList.size()-1)
                segList[i+1].srcCmd = true;
        }
    }
    return (segList.size() != oldSize);
}

bool Path::convertSegments(QList<Segment> &segList)
{
    if (segList.isEmpty())
        return false;
    // TODO: add other commands conv

    bool isAnyChanged = false;
    for (int i = 1; i < segList.size(); ++i) {
        Segment prevSeg = segList.at(i-1);
        Segment seg = segList.at(i);
        const QChar cmd = seg.command;
        bool isChanged = false;
        if (cmd == Command::LineTo) {
            if (isZero(seg.x - prevSeg.x) && seg.y != prevSeg.y) {
                segList[i].command = Command::VerticalLineTo;
                isChanged = true;
            }
            else if (seg.x != prevSeg.x && isZero(seg.y - prevSeg.y)) {
                segList[i].command = Command::HorizontalLineTo;
                isChanged = true;
            }

            // TODO: useful, but do not work well with subpaths
//            else if (prevSeg.command == Command::MoveTo) {
//                segList[i].command = Command::MoveTo;
//                segList[i].srcCmd = false;
//            }
        } else if (cmd == Command::CurveTo) {
            bool isXY1Equal = isZero(seg.x1 - prevSeg.x) && isZero(seg.y1 - prevSeg.y);

            if (   isXY1Equal
                && isZero(seg.x2 - prevSeg.x2)
                && isZero(seg.x - prevSeg.x)
                && qFuzzyCompare(seg.y2, seg.y))
            {
                segList[i].command = Command::VerticalLineTo;
                isChanged = true;
            }
            else if (   isXY1Equal
                     && isZero(seg.y2 - prevSeg.y2)
                     && isZero(seg.y - prevSeg.y)
                     && qFuzzyCompare(seg.x2, seg.x))
            {
                segList[i].command = Command::HorizontalLineTo;
                isChanged = true;
            }
            else if (   isXY1Equal
                     && qFuzzyCompare(seg.y2, seg.y)
                     && qFuzzyCompare(seg.x2, seg.x))
            {
                segList[i].command = Command::LineTo;
                isChanged = true;
            }
            else if (prevSeg.command == Command::CurveTo)
            {
                bool flag1 = qFuzzyCompare(seg.x1, (prevSeg.x - prevSeg.x2))
                                 || (isZero(seg.x1 - prevSeg.x1) && isZero(prevSeg.x - prevSeg.x2));
                bool flag2 = qFuzzyCompare(seg.y1, (prevSeg.y - prevSeg.y2))
                                 || (isZero(seg.y1 - prevSeg.y1) && isZero(prevSeg.y - prevSeg.y2));
                if (flag1 && flag2) {
                    segList[i].command = Command::SmoothCurveTo;
                    isChanged = true;
                }
            }
        }
        if (isChanged) {
            // mark current and next segment as source segment to force it adding to path
            segList[i].srcCmd = true;
            if (i < segList.size()-1)
                segList[i+1].srcCmd = true;

            isAnyChanged = true;
        }
    }
    return isAnyChanged;
}

bool isPointOnLine(const QPointF &p1, const QPointF &p2, const QPointF &point)
{
    float a = (p2.y() - p1.y()) / (p2.x() - p1.x());
    float b = p1.y() - a * p1.x();
    float c = fabs(point.y() - (a * point.x() + b));
    if (isZero(c))
        return true;
    return false;
}

bool Path::joinSegments(QList<Segment> &segList)
{
    if (segList.isEmpty())
        return false;

    const int oldSize = segList.size();
    Segment firstSeg;
    if (!segList.isEmpty())
        firstSeg = segList.first();

    for (int i = 1; i < segList.count(); ++i) {
        Segment prevSeg = segList.at(i-1);
        Segment seg = segList.at(i);
        const int tSize = segList.size();

        if (seg.command == prevSeg.command) {
            if (   seg.command == Command::MoveTo
                || seg.command == Command::VerticalLineTo
                || seg.command == Command::HorizontalLineTo)
            {
                segList[i] = seg;
                segList.removeAt(i-1);
                i--;
            }
        }
        if (seg.command == Command::LineTo) {
            // check is current line segment lies on another line segment
            // if true - we don't need it
            if (i < segList.size()-1 && segList.at(i+1).command == Command::LineTo)
            {
                Segment nextSeg = segList.at(i+1);
                bool flag = isPointOnLine(QPointF(prevSeg.x, prevSeg.y),
                                          QPointF(nextSeg.x, nextSeg.y),
                                          QPointF(seg.x,     seg.y));
                if (flag) {
                    segList.removeAt(i);
                    i--;
                }
            }
        }
        if (tSize != segList.size()) {
            if (i < segList.size()-1)
                segList[i+1].srcCmd = true;
        }
    }
    return (segList.size() != oldSize);
}

void Path::removeStrokeLinecap(QList<Segment> &segList)
{
    if (segList.isEmpty())
        return;

    bool allSubpathsAreClosed = false;
    QPointF prevM(segList.first().x, segList.first().y);
    for (int i = 1; i < segList.size(); ++i) {
        if (segList.at(i).command == Command::ClosePath) {
            if (i != segList.size()-1) {
                prevM = QPointF(segList.at(i+1).x, segList.at(i+1).y);
            } else if (i == segList.size()-1) {
                allSubpathsAreClosed = true;
            }
        }
    }
    // 'stroke-linecap' attribute is useless when no open subpaths in path
    if (allSubpathsAreClosed && m_elem.parentAttribute(AttrId::stroke_dasharray, true).isEmpty())
        m_elem.removeAttribute(AttrId::stroke_linecap);
}

void Path::processSegments(QList<Segment> &segList)
{
    removeStrokeLinecap(segList);
    bool isPathChanged = true;
    while (isPathChanged)
    {
        // TODO: add new key for it
        if (Keys.flag(Key::RemoveUnneededSymbols)) {
            isPathChanged = removeZSegments(segList);
            if (isPathChanged)
                continue;

            isPathChanged = removeUnneededMoveToSegments(segList);
            if (isPathChanged)
                continue;

            isPathChanged = joinSegments(segList);
            if (isPathChanged)
                continue;
        }

        if (Keys.flag(Key::RemoveTinySegments)) {
            isPathChanged = removeTinySegments(segList);
            if (isPathChanged)
                continue;
        }

        if (Keys.flag(Key::ConvertSegments)) {
            isPathChanged = convertSegments(segList);
            if (isPathChanged)
                continue;
        }

        isPathChanged = false;
    }
    removeStrokeLinecap(segList);
}

QString Path::segmentsToPath(const QList<Segment> &segList)
{
    if (segList.size() == 1 || segList.isEmpty())
        return QString();

    static const ushort dot   = '.';
    static const ushort space = ' ';

    CharArray array;

    QChar prevCom;
    bool isPrevComAbs = false;
    const bool isTrim = Keys.flag(Key::RemoveUnneededSymbols);
    bool isPrevPosHasDot = false;
    for (int i = 0; i < segList.count(); ++i) {
        Segment segment = segList.at(i);
        const QChar cmd = segment.command;

        // check is previous command is the same as next
        bool writeCmd = true;
        if (isTrim) {
            if (cmd == prevCom && !prevCom.isNull()) {
                if (segment.absolute == isPrevComAbs
                    && !(segment.srcCmd && cmd == Command::MoveTo))
                    writeCmd = false;
            }
        } else {
            // remove only commands which are not set in original path
            if (!segment.srcCmd)
                writeCmd = false;
        }

        if (writeCmd) {
            if (segment.absolute)
                array += segment.command.toUpper().unicode();
            else
                array += segment.command.unicode();
            if (!isTrim)
                array += space;
        }

        bool round = true;
        if (i+1 < segList.size() && round) {
            if ((cmd == Command::MoveTo || cmd == Command::EllipticalArc)
                && segList.at(i+1).command == Command::EllipticalArc)
                round = false;
        }
        if (cmd == Command::EllipticalArc)
           round = false;

        QVarLengthArray<double, 6> points;
        segment.coords(points);
        if (isTrim) {
            // remove unneeded number separators
            // m 30     to  m30
            // 10,-30   to  10-30
            // 15.1,.5  to  15.1.5
            for (int j = 0; j < points.size(); ++j) {
                const int pos = array.size()-1;
                if (round)
                    doubleToVarArr(array, points.at(j), Keys.coordinatesPrecision());
                else
                    doubleToVarArr(array, points.at(j), 6);

                bool useSep = false;
                if (cmd == Command::EllipticalArc)
                    useSep = true;
                else if (!isPrevPosHasDot && array.at(pos+1) == dot)
                    useSep = true;
                else if (QChar(array.at(pos+1)).isDigit())
                    useSep = true;

                if (j == 0 && writeCmd)
                    useSep = false;

                if (useSep)
                    array.insert(pos+1, space);
                isPrevPosHasDot = false;
                for (int i = pos; i < array.size(); ++i) {
                    if (array.at(i) == dot) {
                        isPrevPosHasDot = true;
                        break;
                    }
                }
            }
        } else {
            for (int j = 0; j < points.size(); ++j) {
                if (round)
                    doubleToVarArr(array, points.at(j), Keys.coordinatesPrecision());
                else
                    doubleToVarArr(array, points.at(j), 6);
                if (j != points.size()-1)
                    array += space;
            }
            if (cmd != Command::ClosePath)
                array += space;
        }
        prevCom = cmd;
        isPrevComAbs = segment.absolute;
    }
    if (!isTrim)
        array.resize(array.size()-1); // remove last char
    // convert ushort array to QString
    return QString(reinterpret_cast<QChar *>(array.data()), array.size());
}

// path bounding box calculating part

// TODO: to separate class

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
QString Path::calcBoundingBox(const QList<Segment> &segList, bool convertToCurves)
{
    // convert all segments to curve, exept m and z
    QList<Segment> tmpSegList = segList;
    if (convertToCurves) {
        Segment prevSegment2;
        for (int i = 1; i < tmpSegList.size(); ++i) {
            Segment prevSegment = tmpSegList.at(i-1);
            Segment currSegment = tmpSegList.at(i);
            QVarLengthArray<Segment, 3> list = tmpSegList.at(i).toCurve(prevSegment, prevSegment2);
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
        Segment seg = tmpSegList.at(i);
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
