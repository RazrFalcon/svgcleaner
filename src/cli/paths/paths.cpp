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

#include "../stringwalker.h"
#include "../tools.h"
#include "pathbbox.h"
#include "paths.h"

#define Keys Keys::get()

using namespace Attribute;
using namespace DefaultValue;

// http://www.w3.org/TR/SVG/paths.html

// TODO: add path approximation
//       fjudy2001_02.svg
//       warszawianka_Betel.svg

void Path::processPath(SvgElement elem, bool canApplyTransform, bool *isTransformApplied)
{
    const QString inPath = elem.attribute(AttrId::d);

    if (inPath.contains(QL1S("nan"))) {
        elem.setAttribute(AttrId::d, QString());
        return;
    }
    m_elem = elem;

    PathSegmentList segList;
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

    PathSegmentList tsSegList = segList;

    if (Keys.flag(Key::RemoveOutsideElements))
        elem.setAttribute(AttrId::bbox, PathBBox::calcBoundingBox(segList, true));

    const bool convToRelative = !(!isOnlyAbsolute && Keys.flag(Key::ConvertToRelative));
    segmentsToRelative(segList, convToRelative);

    // merge segments to path
    QString outPath = segmentsToPath(segList);

    // set old path, if new is longer
    if (outPath.size() > inPath.size())
        outPath = inPath;
    elem.setAttribute(SvgAttribute(segList, outPath));

    // try to bake transforms into path
    if (canApplyTransform) {
        applyTransform(tsSegList);

        if (Keys.flag(Key::RemoveOutsideElements))
            elem.setAttribute(QL1S("bboxts"), PathBBox::calcBoundingBox(tsSegList, false));

        processSegments(tsSegList);
        if (Keys.flag(Key::ConvertToRelative))
            segmentsToRelative(tsSegList, false);
        calcNewStrokeWidth(m_elem.transform().scaleFactor());
        elem.setAttribute(QL1S("dts"), segmentsToPath(tsSegList));

        // apply transform only if it shorter
        if (isTsPathShorter() || Keys.flag(Key::ForceApplyTransformsToPaths)) {
            *isTransformApplied = true;
            elem.setAttribute(SvgAttribute(tsSegList, elem.attribute(QL1S("dts"))));
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
void Path::splitToSegments(const QString &path, PathSegmentList &segList) const
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
                PathSegment seg;
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
            PathSegment seg;
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

PathSegmentList Path::pathToSegments(const QString &path) const
{
    PathSegmentList segList;
    splitToSegments(path, segList);
    return segList;
}

void Path::calcNewStrokeWidth(const double scaleFactor)
{
    SvgElement parentElem = m_elem;
    bool hasParentStrokeWidth = false;
    while (!parentElem.isNull()) {
        if (parentElem.hasAttribute(AttrId::stroke_width)) {
            double strokeWidth
                = toDouble(convertUnitsToPx(parentElem.attribute(AttrId::stroke_width)));
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

void Path::applyTransform(PathSegmentList &tsSegList)
{
    Transform ts = m_elem.transform();

    PathSegment prevSegment2;
    for (int i = 1; i < tsSegList.count(); ++i) {
        PathSegment prevSegment = tsSegList.at(i-1);
        PathSegment currSeg = tsSegList.at(i);
        QVarLengthArray<PathSegment, 3> list = tsSegList.at(i).toCurve(prevSegment, prevSegment2);
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

void Path::segmentsToRelative(PathSegmentList &segList, bool onlyIfSourceWasRelative)
{
    double lastX = 0;
    double lastY = 0;
    double lastMX = segList.first().x;
    double lastMY = segList.first().y;
    for (int i = 0; i < segList.count(); ++i) {
        PathSegment segment = segList.at(i);
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

void Path::processSegments(PathSegmentList &segList)
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

bool isLineBasedSegment(const PathSegment &seg)
{
    return    seg.command == Command::LineTo
           || seg.command == Command::HorizontalLineTo
           || seg.command == Command::VerticalLineTo;
}

bool Path::removeZSegments(PathSegmentList &segList)
{
    if (segList.isEmpty())
        return false;

    // remove 'z' segment if last 'm' segment equal to segment before 'z'

    bool isRemoved = false;
    QString stroke = m_elem.parentAttribute(AttrId::stroke, true);
    bool hasStroke = (!stroke.isEmpty() && stroke != V_none);

    QPointF prevM(segList.first().x, segList.first().y);
    for (int i = 1; i < segList.size(); ++i) {
        if (segList.at(i).command != Command::ClosePath)
            continue;

        PathSegment prevSeg = segList.at(i-1);
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

bool Path::removeUnneededMoveToSegments(PathSegmentList &segList)
{
    if (segList.isEmpty())
        return false;

    // remove repeated MoveTo segments, if it was absolute in original path
    bool isRemoved = false;
    for (int i = 1; i < segList.count(); ++i) {
        PathSegment prevSeg = segList.at(i-1);
        PathSegment seg = segList.at(i);
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
bool Path::removeTinySegments(PathSegmentList &segList)
{
    if (segList.isEmpty())
        return false;

    const int oldSize = segList.size();
    static double minValue = 1 / pow(10, Keys.coordinatesPrecision());

    // if current segment too close to previous - remove it
    for (int i = 0; i < segList.count()-1; ++i) {
        PathSegment seg = segList.at(i);
        PathSegment nextSeg = segList.at(i+1);
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
        PathSegment prevSeg = segList.at(i-1);
        PathSegment seg = segList.at(i);
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

bool Path::convertSegments(PathSegmentList &segList)
{
    if (segList.isEmpty())
        return false;
    // TODO: add other commands conv

    bool isAnyChanged = false;
    for (int i = 1; i < segList.size(); ++i) {
        PathSegment prevSeg = segList.at(i-1);
        PathSegment seg = segList.at(i);
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

bool Path::joinSegments(PathSegmentList &segList)
{
    if (segList.isEmpty())
        return false;

    const int oldSize = segList.size();
    PathSegment firstSeg;
    if (!segList.isEmpty())
        firstSeg = segList.first();

    for (int i = 1; i < segList.count(); ++i) {
        PathSegment prevSeg = segList.at(i-1);
        PathSegment seg = segList.at(i);
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
                PathSegment nextSeg = segList.at(i+1);
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

void Path::removeStrokeLinecap(PathSegmentList &segList)
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

QString Path::segmentsToPath(const PathSegmentList &segList) const
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
        PathSegment segment = segList.at(i);
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
