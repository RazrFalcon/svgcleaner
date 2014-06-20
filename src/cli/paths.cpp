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
    const QChar MoveTo           = 'm';
    const QChar LineTo           = 'l';
    const QChar HorizontalLineTo = 'h';
    const QChar VerticalLineTo   = 'v';
    const QChar CurveTo          = 'c';
    const QChar SmoothCurveTo    = 's';
    const QChar Quadratic        = 'q';
    const QChar SmoothQuadratic  = 't';
    const QChar EllipticalArc    = 'a';
    const QChar ClosePath        = 'z';
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

QPointF Segment::rotatePoint(qreal x, qreal y, qreal rad) const
{
    qreal X = x * cos(rad) - y * sin(rad);
    qreal Y = x * sin(rad) + y * cos(rad);
    return QPointF(X, Y);
}

// arcToCurve is a port from Raphael JavaScript library to Qt (MIT license)
QList<QPointF> Segment::arcToCurve(ArcStruct arc) const
{
    qreal f1 = 0;
    qreal f2 = 0;
    qreal cx = 0;
    qreal cy = 0;
    qreal rad = M_PI / 180 * arc.angle;
    if (arc.recursive.isEmpty()) {
        QPointF p = rotatePoint(arc.x1, arc.y1, -rad);
        arc.x1 = p.x();
        arc.y1 = p.y();
        p = rotatePoint(arc.x2, arc.y2, -rad);
        arc.x2 = p.x();
        arc.y2 = p.y();

        qreal x = (arc.x1 - arc.x2) / 2;
        qreal y = (arc.y1 - arc.y2) / 2;
        qreal h = (x * x) / (arc.rx * arc.rx) + (y * y) / (arc.ry * arc.ry);
        if (h > 1) {
            h = sqrt(h);
            arc.rx = h * arc.rx;
            arc.ry = h * arc.ry;
        }

        qreal rx2 = arc.rx * arc.rx;
        qreal ry2 = arc.ry * arc.ry;
        qreal kk = 1;
        if (arc.large_arc_flag == arc.sweep_flag)
            kk = -1;
        qreal k = kk * sqrt(fabs((rx2 * ry2 - rx2 * y * y - ry2 * x * x)
                                     / (rx2 * y * y + ry2 * x * x)));
        cx = k * arc.rx * y / arc.ry + (arc.x1 + arc.x2) / 2;
        cy = k * -arc.ry * x / arc.rx + (arc.y1 + arc.y2) / 2;

        qreal tt = (arc.y1 - cy) / arc.ry;
        if (tt > 1)
            tt = 1;
        if (tt < -1)
            tt = -1;
        f1 = asin(tt);

        qreal tt2 = (arc.y2 - cy) / arc.ry;
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

    QList<QPointF> res;
    qreal df = f2 - f1;
    const qreal a90 = M_PI * 90 / 180;
    if (abs(df) > a90) {
        qreal f2old = f2;
        qreal x2old = arc.x2;
        qreal y2old = arc.y2;
        qreal c = -1;
        if (arc.sweep_flag && f2 > f1)
            c = 1;
        f2 = f1 + a90 * c;
        arc.x2 = cx + arc.rx * cos(f2);
        arc.y2 = cy + arc.ry * sin(f2);
        QList<qreal> list;
        list.reserve(4);
        list << f2 << f2old << cx << cy;
        ArcStruct newArk = { arc.x2, arc.y2, arc.rx, arc.ry, arc.angle, 0,
                             arc.sweep_flag, x2old, y2old, list };
        res = arcToCurve(newArk);
    }

    df = f2 - f1;
    qreal c1 = cos(f1);
    qreal s1 = sin(f1);
    qreal c2 = cos(f2);
    qreal s2 = sin(f2);
    qreal t = tan(df / 4.0);
    qreal hx = 4.0 / 3.0 * arc.rx * t;
    qreal hy = 4.0 / 3.0 * arc.ry * t;

    QPointF p1(arc.x1, arc.y1);
    QPointF p2(arc.x1 + hx * s1, arc.y1 - hy * c1);
    QPointF p3(arc.x2 + hx * s2, arc.y2 - hy * c2);
    QPointF p4(arc.x2, arc.y2);

    p2.setX(2 * p1.x() - p2.x());
    p2.setY(2 * p1.y() - p2.y());

    QList<QPointF> list;
    list.reserve(4);
    list << p2 << p3 << p4 << res;
    return list;
}

// FIXME: add s, q, t segments
QList<Segment> Segment::toCurve(qreal prevX, qreal prevY) const
{
    QList<Segment> segList;
    if (command == Command::LineTo)
    {
        Segment seg;
        seg.command = Command::CurveTo;
        seg.absolute = true;
        seg.srcCmd = false;
        seg.x1 = prevX;
        seg.y1 = prevY;
        seg.x2 = x;
        seg.y2 = y;
        seg.x  = x;
        seg.y  = y;
        segList.reserve(1);
        segList.append(seg);
    }
    else if (command == Command::EllipticalArc)
    {
        if (!isZero(rx) && !isZero(ry)) {
            ArcStruct arc = { prevX, prevY, rx, ry, xAxisRotation, largeArc,
                              sweep, x, y, QList<qreal>() };
            QList<QPointF> list = arcToCurve(arc);
            segList.reserve(list.size()/3);
            for (int i = 0; i < list.size(); i += 3) {
                Segment seg;
                seg.command = Command::CurveTo;
                seg.absolute = true;
                seg.srcCmd = false;
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

void Segment::toRelative(qreal xLast, qreal yLast)
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

void Segment::coords(QVector<qreal> &points)
{
    points.reserve(7);
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

// New class

void Path::processPath(SvgElement elem, bool canApplyTransform, bool *isPathApplyed)
{
    const QString inPath = elem.attribute(AttrId::d);

    if (inPath.contains("nan")) {
        elem.setAttribute(AttrId::d, "");
        return;
    }
    m_elem = elem;

    QList<Segment> segList;
    splitToSegments(inPath.midRef(0), segList);
    // paths without segments or with first segment not 'moveto' are invalid
    if (segList.isEmpty() || segList.first().command != Command::MoveTo) {
        if (Keys.flag(Key::RemoveInvisibleElements))
            elem.setAttribute(AttrId::d, "");
        return;
    }

    // path like 'M x,y A rx,ry 0 1 1 x,y', where x and y of both segments are similar
    // can't be converted to relative coordinates
    // TODO: MA could be in subpath
    bool isOnlyAbsolute = false;
    if (segList.at(1).command == Command::EllipticalArc)
        isOnlyAbsolute = true;

    if (Keys.flag(Key::RemoveOutsideElements)) {
        if (!elem.hasAttribute(AttrId::bbox))
            calcBoundingBox(segList);
    }

    processSegments(segList);
    if (canApplyTransform && !isOnlyAbsolute)
        canApplyTransform = applyTransform(segList);
    if (!isOnlyAbsolute && Keys.flag(Key::ConvertToRelative))
        segmentsToRelative(segList, false);
    else
        segmentsToRelative(segList, true);

    // merge segments to path
    QString outPath = segmentsToPath(segList);

    // set old path, if new is longer
    if (outPath.size() > inPath.size())
        outPath = inPath;
    elem.setAttribute(AttrId::d, outPath);

    if (canApplyTransform && !isOnlyAbsolute) {
        if (isTsPathShorter()) {
            *isPathApplyed = true;
            elem.setAttribute(AttrId::d, elem.attribute("dts"));
            QString newStroke = elem.attribute("stroke-width-new");
            if (!newStroke.isEmpty() && newStroke != "1")
                elem.setAttribute(AttrId::stroke_width, newStroke);
            else
                elem.removeAttribute(AttrId::stroke_width);
        }
        elem.removeAttribute("stroke-width-new");
        elem.removeAttribute("dts");
    }
}

void Path::splitToSegments(const QStringRef &path, QList<Segment> &segList)
{
    const QChar *str = path.constData();
    const QChar *end = str + path.size();
    QChar cmd;
    QChar prevCmd;
    bool isNewCmd = false;
    qreal prevX = 0;
    qreal prevY = 0;
    qreal prevMX = 0;
    qreal prevMY = 0;
    while (str != end) {
        while (str->isSpace())
            ++str;
        QChar pathElem = *str;
        if (pathElem.isLetter()) {
            isNewCmd = true;
            if (pathElem == Command::ClosePath) {
                Segment seg;
                seg.command = Command::ClosePath;
                seg.absolute = false;
                seg.srcCmd = true;
                segList << seg;
                prevX = prevMX;
                prevY = prevMY;
            }
            prevCmd = cmd.toLower();
            cmd = pathElem;
            ++str;
        } else if (cmd != Command::ClosePath) {
            qreal offsetX = 0;
            qreal offsetY = 0;
            Segment seg;
            seg.command = cmd.toLower();
            seg.absolute = cmd.isUpper();
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
                seg.x = getNum(str) + offsetX;
                seg.y = getNum(str) + offsetY;
                if (seg.srcCmd) {
                    prevMX = seg.x;
                    prevMY = seg.y;
                }
            } else if (   seg.command == Command::LineTo
                       || seg.command == Command::SmoothQuadratic) {
                seg.x = getNum(str) + offsetX;
                seg.y = getNum(str) + offsetY;
            } else if (seg.command == Command::HorizontalLineTo) {
                seg.command = Command::LineTo;
                seg.x = getNum(str) + offsetX;
                seg.y = segList.last().y;
            } else if (seg.command == Command::VerticalLineTo) {
                seg.command = Command::LineTo;
                seg.x = segList.last().x;
                seg.y = getNum(str) + offsetY;
            } else if (seg.command == Command::CurveTo) {
                seg.x1 = getNum(str) + offsetX;
                seg.y1 = getNum(str) + offsetY;
                seg.x2 = getNum(str) + offsetX;
                seg.y2 = getNum(str) + offsetY;
                seg.x  = getNum(str) + offsetX;
                seg.y  = getNum(str) + offsetY;
            } else if (seg.command == Command::SmoothCurveTo) {
                seg.x2 = getNum(str) + offsetX;
                seg.y2 = getNum(str) + offsetY;
                seg.x  = getNum(str) + offsetX;
                seg.y  = getNum(str) + offsetY;
            } else if (seg.command == Command::Quadratic) {
                seg.x1 = getNum(str) + offsetX;
                seg.y1 = getNum(str) + offsetY;
                seg.x  = getNum(str) + offsetX;
                seg.y  = getNum(str) + offsetY;
            } else if (seg.command == Command::EllipticalArc) {
                seg.rx = getNum(str);
                seg.ry = getNum(str);
                seg.xAxisRotation = getNum(str);
                seg.largeArc      = getNum(str);
                seg.sweep         = getNum(str);
                seg.x = getNum(str) + offsetX;
                seg.y = getNum(str) + offsetY;
            }
            prevX = seg.x;
            prevY = seg.y;
            segList << seg;
            prevCmd = cmd.toLower();
        }
    }
    if (segList.size() == 1)
        segList.clear();
}

void Path::calcNewStrokeWidth(const Transform &transform)
{
    SvgElement parentElem = m_elem;
    bool hasParentStrokeWidth = false;
    while (!parentElem.isNull()) {
        if (parentElem.hasAttribute(AttrId::stroke_width)) {
            qreal strokeWidth
                    = Tools::convertUnitsToPx(parentElem.attribute(AttrId::stroke_width)).toDouble();
            QString sw = roundNumber(strokeWidth * transform.scaleFactor(), Round::Attribute);
            m_elem.setAttribute("stroke-width-new", sw);
            hasParentStrokeWidth = true;
            break;
        }
        parentElem = parentElem.parentElement();
    }
    if (!hasParentStrokeWidth) {
        m_elem.setAttribute("stroke-width-new", roundNumber(transform.scaleFactor(),
                                                                   Round::Attribute));
    }
}

bool Path::applyTransform(QList<Segment> &segList)
{
    QString transStr = m_elem.attribute(AttrId::transform);
    QList<Segment> tsSegList = segList;
    Transform ts(transStr);
    for (int i = 1; i < tsSegList.count(); ++i) {
        Segment prevSegment = tsSegList.at(i-1);
        QList<Segment> list = tsSegList.at(i).toCurve(prevSegment.x, prevSegment.y);
        if (!list.isEmpty()) {
            tsSegList.removeAt(i);
            for (int j = 0; j < list.count(); ++j)
                tsSegList.insert(i+j, list.at(j));
        }
    }
    for (int i = 0; i < tsSegList.count(); ++i)
        tsSegList[i].setTransform(ts);

    if (Keys.flag(Key::ConvertToRelative))
        segmentsToRelative(tsSegList, false);
    calcNewStrokeWidth(ts);
    m_elem.setAttribute("dts", segmentsToPath(tsSegList));
    return true;
}

bool Path::isTsPathShorter()
{
    quint32 afterTransform = m_elem.attribute("dts").size();
    if (m_elem.hasAttribute("stroke-width-new")) {
        afterTransform += m_elem.attribute("stroke-width-new").size();
        // char count in ' stroke-width=\"\" '
        afterTransform += 17;
    }

    quint32 beforeTransform = m_elem.attribute(AttrId::d).size();
    if (m_elem.hasAttribute(AttrId::transform)) {
        beforeTransform += m_elem.attribute(AttrId::transform).size();
        // char count in ' transform=\"\" '
        beforeTransform += 14;
    }
    if (m_elem.hasAttribute(AttrId::stroke_width)) {
        beforeTransform += m_elem.attribute(AttrId::stroke_width).size();
        // char count in ' stroke-width=\"\" '
        beforeTransform += 17;
    }

    return (afterTransform < beforeTransform);
}

void Path::segmentsToRelative(QList<Segment> &segList, bool onlyIfSourceWasRelative)
{
    qreal lastX = 0;
    qreal lastY = 0;
    qreal lastMX = segList.first().x;
    qreal lastMY = segList.first().y;
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

QString Path::findAttribute(const int &attrId)
{
    if (m_elem.isNull())
        return "";
    SvgElement parent = m_elem;
    while (!parent.isNull()) {
        if (parent.hasAttribute(attrId))
            return parent.attribute(attrId);
        parent = parent.parentElement();
    }
    return "";
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
    QString stroke = findAttribute(AttrId::stroke);
    QString strokeLinecap = findAttribute(AttrId::stroke_linecap);
    bool hasStroke = (!stroke.isEmpty() && stroke != V_none);
    bool isDefaultLinecap = (strokeLinecap.isEmpty() || strokeLinecap == QL1S("butt"));

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

                Segment seg = segList.at(i);
                if (isDefaultLinecap && isLineBasedSegment(seg)) {
                    // remove 'z' segment if we have path like:
                    // M 80 90 L 200 90 Z
                    // but only when 'stroke-linecap' = 'butt'
                    // in other cases it will change rendering of the path
                    if (segList.size() == 3 || (i - lastMIdx == 2)) {
                        segList.removeAt(i);
                        i--;
                    }
                }
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

bool Path::removeTinySegments(QList<Segment> &segList)
{
    if (segList.isEmpty())
        return false;

    const int oldSize = segList.size();
    static qreal minValue = 1 / pow(10, Keys.coordinatesPrecision());

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

    bool isElemHasFilter = !findAttribute(AttrId::filter).isEmpty();
    for (int i = 1; i < segList.count(); ++i) {
        Segment prevSeg = segList.at(i-1);
        Segment seg = segList.at(i);
        const QChar cmd = seg.command;
        const int tsize = segList.size();
        if (isZero(seg.x - prevSeg.x) && isZero(seg.y - prevSeg.y)) {
            if (cmd == Command::MoveTo) {
                // removing MoveTo from path with blur filter breaks render
                if (!isElemHasFilter && prevSeg.command == Command::MoveTo)
                {
                    segList.removeAt(i--);
                } else if (i == segList.size()-1 && prevSeg.command != Command::MoveTo) {
                    segList.removeAt(i--);
                }
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
            bool isXY1Equal = isZero(seg.x1 - prevSeg.x1) && isZero(seg.y1 - prevSeg.y1);
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
    if (allSubpathsAreClosed && findAttribute(AttrId::stroke_dasharray).isEmpty())
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

QString Path::segmentsToPath(QList<Segment> &segList)
{
    if (segList.size() == 1 || segList.isEmpty())
        return "";

    static const ushort dot   = QL1C('.').unicode();
    static const ushort space = QL1C(' ').unicode();

    QVarLengthArray<ushort> array;

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
            // remove only commands which is not set in original path
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

        QVector<qreal> points;
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

struct Box
{
    qreal minx;
    qreal miny;
    qreal maxx;
    qreal maxy;
};

struct Bezier
{
    qreal p_x;
    qreal p_y;
    qreal x1;
    qreal y1;
    qreal x2;
    qreal y2;
    qreal x;
    qreal y;
};

void updateBBox(Box &bbox, qreal x, qreal y)
{
    bbox.maxx = qMax(x, bbox.maxx);
    bbox.minx = qMin(x, bbox.minx);
    bbox.maxy = qMax(y, bbox.maxy);
    bbox.miny = qMin(y, bbox.miny);
}

QPointF findDotOnCurve(const Bezier &b, qreal t)
{
    qreal t1 = 1 - t;
    qreal t_2 = t * t;
    qreal t_3 = t_2 * t;
    qreal t1_2 = t1 * t1;
    qreal t1_3 = t1_2 * t1;
    qreal x =   t1_3 * b.p_x
              + t1_2 * 3 * t * b.x1
              + t1 * 3 * t_2 * b.x2
              + t_3 * b.x;
    qreal y =   t1_3 * b.p_y
              + t1_2 * 3 * t * b.y1
              + t1 * 3 * t_2 * b.y2
              + t_3 * b.y;
    return QPointF(x, y);
}

void curveExtremum(qreal a, qreal b, qreal c, const Bezier &bezier, Box &bbox)
{
    qreal t1 = (-b + sqrt(b * b - 4 * a * c)) / 2 / a;
    qreal t2 = (-b - sqrt(b * b - 4 * a * c)) / 2 / a;

    if (t1 > 0 && t1 < 1) {
        QPointF p = findDotOnCurve(bezier, t1);
        updateBBox(bbox, p.x(), p.y());
    }
    if (t2 > 0 && t2 < 1) {
        QPointF p = findDotOnCurve(bezier, t2);
        updateBBox(bbox, p.x(), p.y());
    }
}

void curveBBox(const Bezier &bz, Box &bbox)
{
    updateBBox(bbox, bz.p_x, bz.p_y);
    updateBBox(bbox, bz.x, bz.y);

    qreal a = (bz.x2 - 2 * bz.x1 + bz.p_x) - (bz.x - 2 * bz.x2 + bz.x1);
    qreal b = 2 * (bz.x1 - bz.p_x) - 2 * (bz.x2 - bz.x1);
    qreal c = bz.p_x - bz.x1;
    curveExtremum(a, b, c, bz, bbox);

    a = (bz.y2 - 2 * bz.y1 + bz.p_y) - (bz.y - 2 * bz.y2 + bz.y1);
    b = 2 * (bz.y1 - bz.p_y) - 2 * (bz.y2 - bz.y1);
    c = bz.p_y - bz.y1;
    curveExtremum(a, b, c, bz, bbox);
}

void Path::calcBoundingBox(const QList<Segment> &segList)
{
    // convert all segments to curve, exept m and z
    QList<Segment> tmpSegList = segList;
    for (int i = 1; i < tmpSegList.size(); ++i) {
        Segment prevSegment = tmpSegList.at(i-1);
        QList<Segment> list = tmpSegList.at(i).toCurve(prevSegment.x, prevSegment.y);
        if (!list.isEmpty()) {
            tmpSegList.removeAt(i);
            for (int j = 0; j < list.count(); ++j)
                tmpSegList.insert(i+j, list.at(j));
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
    m_elem.setAttribute(AttrId::bbox,
                                  roundNumber(bbox.minx)
                          + " " + roundNumber(bbox.miny)
                          + " " + roundNumber(bbox.maxx - bbox.minx)
                          + " " + roundNumber(bbox.maxy - bbox.miny));
}
