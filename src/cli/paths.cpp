/****************************************************************************
**
** SVG Cleaner is batch, tunable, crossplatform SVG cleaning program.
** Copyright (C) 2013 Evgeniy Reizner
** Copyright (C) 2012 Andrey Bayrak, Evgeniy Reizner
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

#include <QStringBuilder>

#include <cmath>

#include "tools.h"
#include "paths.h"
#include "keys.h"

// http://www.w3.org/TR/SVG/paths.html

// TODO: add path approximation
// fjudy2001_02_cleaned
// warszawianka_Betel.svg

Segment::Segment()
{
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

// arcToCurve is a port from 'raphaeljs' to Qt (MIT license)
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

QList<Segment> Segment::toCurve(qreal prevX, qreal prevY) const
{
    QList<Segment> segList;
    if (   command == Command::LineTo
        || command == Command::HorizontalLineTo
        || command == Command::VerticalLineTo)
    {
        Segment seg;
        seg.command = Command::CurveTo;
        seg.absolute = true;
        seg.srcCmd = false;
        seg.x1 = prevX;
        seg.y1 = prevY;
        if (command == Command::HorizontalLineTo) {
            seg.x2 = x;
            seg.y2 = prevY;
            seg.x  = x;
            seg.y  = prevY;
        } else if (command == Command::VerticalLineTo) {
            seg.x2 = prevX;
            seg.y2 = y;
            seg.x  = prevX;
            seg.y  = y;
        } else {
            seg.x2 = x;
            seg.y2 = y;
            seg.x  = x;
            seg.y  = y;
        }
        segList.append(seg);
    }
    else if (command == Command::EllipticalArc)
    {
        if (!Tools::isZero(rx) && !Tools::isZero(ry)) {
            ArcStruct arc = { prevX, prevY, rx, ry, xAxisRotation, largeArc,
                              sweep, x, y, QList<qreal>() };
            QList<QPointF> list = arcToCurve(arc);
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

void Segment::toAbsolute(qreal xLast, qreal yLast)
{
    if (absolute)
        return;

    if (command == Command::HorizontalLineTo) {
        x = x + xLast;
    } else if (command == Command::VerticalLineTo) {
        y = y + yLast;
    } else {
        x = x + xLast;
        y = y + yLast;
        if (command == Command::CurveTo) {
            x1 = x1 + xLast;
            y1 = y1 + yLast;
            x2 = x2 + xLast;
            y2 = y2 + yLast;
        } else if (command == Command::SmoothCurveTo) {
            x2 = x2 + xLast;
            y2 = y2 + yLast;
        } else if (command == Command::Quadratic) {
            x1 = x1 + xLast;
            y1 = y1 + yLast;
        }
    }
    absolute = true;
}

void Segment::toRelative(qreal xLast, qreal yLast)
{
    if (!absolute)
        return;

    if (command == Command::HorizontalLineTo) {
        x = x - xLast;
    } else if (command == Command::VerticalLineTo) {
        y = y - yLast;
    } else {
        x = x - xLast;
        y = y - yLast;
        if (command == Command::CurveTo) {
            x1 = x1 - xLast;
            y1 = y1 - yLast;
            x2 = x2 - xLast;
            y2 = y2 - yLast;
        } else if (command == Command::SmoothCurveTo) {
            x2 = x2 - xLast;
            y2 = y2 - yLast;
        } else if (command == Command::Quadratic) {
            x1 = x1 - xLast;
            y1 = y1 - yLast;
        }
    }
    absolute = false;
}

void Segment::coords(QVector<qreal> &points)
{
    if (command == Command::CurveTo)
    {
        points.reserve(6);
        points << x1 << y1 << x2 << y2 << x << y;
    }
    else if (  command == Command::MoveTo
            || command == Command::LineTo
            || command == Command::SmoothQuadratic)
    {
        points.reserve(2);
        points << x << y;
    }
    else if (command == Command::HorizontalLineTo)
    {
        points.reserve(1);
        points << x;
    }
    else if (command == Command::VerticalLineTo)
    {
        points.reserve(1);
        points << y;
    }
    else if (command == Command::SmoothCurveTo)
    {
        points.reserve(4);
        points << x2 << y2 << x << y;
    }
    else if (command == Command::Quadratic)
    {
        points.reserve(4);
        points << x1 << y1 << x << y;
    }
    else if (command == Command::EllipticalArc)
    {
        points.reserve(7);
        points << rx << ry << xAxisRotation << largeArc << sweep << x << y;
    }
}


// New class

Q_CORE_EXPORT double qstrtod(const char *s00, char const **se, bool *ok);

PathParser::PathParser(const QStringRef &d)
{
    splitToSegments(d);
}

void PathParser::splitToSegments(const QStringRef &path)
{
    const QChar *str = path.constData();
    const QChar *end = str + path.size();
    QChar prevCmd;
    bool newCmd = false;
    while (str != end) {
        while (str->isSpace())
            ++str;
        QChar pathElem = *str;
        if (pathElem.isLetter()) {
            newCmd = true;
            if (pathElem == Command::ClosePath) {
                Segment seg;
                seg.command = Command::ClosePath;
                seg.absolute = false;
                seg.srcCmd = true;
                m_segList << seg;
            }
            prevCmd = pathElem;
            ++str;
        } else if (prevCmd != Command::ClosePath) {
            Segment seg;
            seg.command = prevCmd.toLower();
            seg.absolute = prevCmd.isUpper();
            seg.srcCmd = newCmd;
            newCmd = false;
            if (   seg.command == Command::MoveTo
                || seg.command == Command::LineTo
                || seg.command == Command::SmoothQuadratic) {
                seg.x = getNum(str);
                seg.y = getNum(str);
            } else if (seg.command == Command::HorizontalLineTo) {
                seg.x = getNum(str);
                seg.y = 0;
            } else if (seg.command == Command::VerticalLineTo) {
                seg.x = 0;
                seg.y = getNum(str);
            } else if (seg.command == Command::CurveTo) {
                seg.x1 = getNum(str);
                seg.y1 = getNum(str);
                seg.x2 = getNum(str);
                seg.y2 = getNum(str);
                seg.x  = getNum(str);
                seg.y  = getNum(str);
            } else if (seg.command == Command::SmoothCurveTo) {
                seg.x2 = getNum(str);
                seg.y2 = getNum(str);
                seg.x  = getNum(str);
                seg.y  = getNum(str);
            } else if (seg.command == Command::Quadratic) {
                seg.x1 = getNum(str);
                seg.y1 = getNum(str);
                seg.x  = getNum(str);
                seg.y  = getNum(str);
            } else if (seg.command == Command::EllipticalArc) {
                seg.rx = getNum(str);
                seg.ry = getNum(str);
                seg.xAxisRotation = getNum(str);
                seg.largeArc      = getNum(str);
                seg.sweep         = getNum(str);
                seg.x = getNum(str);
                seg.y = getNum(str);
            }
            m_segList << seg;
        }
    }
    if (m_segList.size() == 1)
        m_segList.clear();
}

// the toDouble code underneath is from QtSvg module (qsvghandler.cpp) (LGPLv2 license)
qreal PathParser::toDouble(const QChar *&str)
{
    const int maxLen = 255; // technically doubles can go til 308+ but whatever
    char temp[maxLen+1];
    int pos = 0;

    if (*str == QLatin1Char('-')) {
        temp[pos++] = '-';
        ++str;
    } else if (*str == QLatin1Char('+')) {
        ++str;
    }
    while (isDigit(str->unicode()) && pos < maxLen) {
        temp[pos++] = str->toLatin1();
        ++str;
    }
    if (*str == QLatin1Char('.') && pos < maxLen) {
        temp[pos++] = '.';
        ++str;
    }
    while (isDigit(str->unicode()) && pos < maxLen) {
        temp[pos++] = str->toLatin1();
        ++str;
    }
    bool exponent = false;
    if ((*str == QLatin1Char('e') || *str == QLatin1Char('E')) && pos < maxLen) {
        exponent = true;
        temp[pos++] = 'e';
        ++str;
        if ((*str == QLatin1Char('-') || *str == QLatin1Char('+')) && pos < maxLen) {
            temp[pos++] = str->toLatin1();
            ++str;
        }
        while (isDigit(str->unicode()) && pos < maxLen) {
            temp[pos++] = str->toLatin1();
            ++str;
        }
    }

    temp[pos] = '\0';

    qreal val;
    if (!exponent && pos < 10) {
        int ival = 0;
        const char *t = temp;
        bool neg = false;
        if(*t == '-') {
            neg = true;
            ++t;
        }
        while(*t && *t != '.') {
            ival *= 10;
            ival += (*t) - '0';
            ++t;
        }
        if(*t == '.') {
            ++t;
            int div = 1;
            while(*t) {
                ival *= 10;
                ival += (*t) - '0';
                div *= 10;
                ++t;
            }
            val = ((qreal)ival)/((qreal)div);
        } else {
            val = ival;
        }
        if (neg)
            val = -val;
    } else {
#if defined(Q_WS_QWS) && !defined(Q_OS_VXWORKS)
        if(sizeof(qreal) == sizeof(float))
            val = strtof(temp, 0);
        else
#endif
        {
            bool ok = false;
            val = qstrtod(temp, 0, &ok);
        }
    }
    return val;
}

qreal PathParser::getNum(const QChar *&str)
{
    while (str->isSpace())
        ++str;
    qreal num = toDouble(str);
    while (str->isSpace())
        ++str;
    if (*str == QLatin1Char(','))
        ++str;
    return num;
}

QList<Segment> PathParser::segmentList() const
{
    return m_segList;
}


// New class

// TODO: try to apply simple transfrorm to simple path without converting it to curves
// TODO: check is absolute path is smaller than relative
void Path::processPath(SvgElement elem, bool canApplyTransform, bool *isPathApplyed)
{
    const QString inPath = elem.attribute("d");

    if (inPath.contains("nan")) {
        elem.setAttribute("d", "");
        return;
    }
    m_elem = elem;

    // TODO: path like 'M x,y A rx,ry 0 1 1 x,y', where x and y of both segments are identical
    // can't be converted to relative coordinates
    // some kind of bug
    bool isOnlyAbsolute = false;
    if (inPath.contains("A")) {
        QString tmpStr = QString(inPath).remove(QRegExp("[^a-AZ-z]"));
        if (tmpStr.contains("MA"))
            isOnlyAbsolute = true;
    }

    QList<Segment> segList = PathParser(inPath.midRef(0)).segmentList();
    if (segList.isEmpty()) {
        elem.setAttribute("d", "");
        return;
    }

    // NOTE: didn't work, in some cases, without segmentsToRelative()
    segmentsToAbsolute(segList);
    processAbsoluteSegments(segList);
    if (canApplyTransform && !isOnlyAbsolute)
        canApplyTransform = applyTransform(segList);
    if (!isOnlyAbsolute) {
        segmentsToRelative(segList);
        processRelativeSegments(segList);
    }

    // merge segments to path
    QString outPath = segmentsToPath(segList);

    // set old path, if new is longer
    if (outPath.size() > inPath.size())
        outPath = inPath;
    elem.setAttribute("d", outPath);

    if (canApplyTransform && !isOnlyAbsolute) {
        if (isTsPathShorter()) {
            *isPathApplyed = true;
            elem.setAttribute("d", elem.attribute("dts"));
            QString newStroke = elem.attribute("stroke-width-new");
            if (!newStroke.isEmpty() && newStroke != "1")
                elem.setAttribute("stroke-width", newStroke);
            else
                elem.removeAttribute("stroke-width");
        }
        elem.removeAttribute("stroke-width-new");
        elem.removeAttribute("dts");
    }
}

void Path::calcNewStrokeWidth(const Transform &transform)
{
    SvgElement parentElem = m_elem;
    bool hasParentStrokeWidth = false;
    while (!parentElem.isNull()) {
        if (parentElem.hasAttribute("stroke-width")) {
            qreal strokeWidth
                    = Tools::convertUnitsToPx(parentElem.attribute("stroke-width")).toDouble();
            QString sw = Tools::roundNumber(strokeWidth * transform.scaleFactor(), Tools::ATTRIBUTE);
            m_elem.setAttribute("stroke-width-new", sw);
            hasParentStrokeWidth = true;
            break;
        }
        parentElem = parentElem.parentNode();
    }
    if (!hasParentStrokeWidth) {
        m_elem.setAttribute("stroke-width-new", Tools::roundNumber(transform.scaleFactor(),
                                                                   Tools::ATTRIBUTE));
    }
}

bool Path::applyTransform(QList<Segment> &segList)
{
    QString transStr = m_elem.attribute("transform");
    QList<Segment> tsSegList = segList;
    Transform ts(transStr);
    for (int i = 1; i < tsSegList.count(); ++i) {
        Segment prevSegment = tsSegList.at(i-1);
        qreal x1 = prevSegment.x;
        qreal y1 = prevSegment.y;
        if (prevSegment.command == Command::HorizontalLineTo)
            y1 = tsSegList.at(i-2).y;
        else if (prevSegment.command == Command::VerticalLineTo)
            x1 = tsSegList.at(i-2).x;
        QList<Segment> list = tsSegList.at(i).toCurve(x1, y1);
        if (!list.isEmpty()) {
            tsSegList.removeAt(i);
            for (int j = 0; j < list.count(); ++j)
                tsSegList.insert(i+j, list.at(j));
        }
    }
    for (int i = 0; i < tsSegList.count(); ++i)
        tsSegList[i].setTransform(ts);

    segmentsToRelative(tsSegList);
    processRelativeSegments(tsSegList);
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

    quint32 beforeTransform = m_elem.attribute("d").size();
    if (m_elem.hasAttribute("transform")) {
        beforeTransform += m_elem.attribute("transform").size();
        // char count in ' transform=\"\" '
        beforeTransform += 14;
    }
    if (m_elem.hasAttribute("stroke-width")) {
        beforeTransform += m_elem.attribute("stroke-width").size();
        // char count in ' stroke-width=\"\" '
        beforeTransform += 17;
    }

    return (afterTransform < beforeTransform);
}

void Path::segmentsToAbsolute(QList<Segment> &segList)
{
    qreal lastX = 0;
    qreal lastY = 0;
    for (int i = 0; i < segList.count(); ++i) {
        Segment segment = segList.at(i);
        if (segment.command != Command::ClosePath) {
            if (segment.absolute) {
                qreal x = segment.x;
                qreal y = segment.y;
                if (x == 0 && segment.command == Command::VerticalLineTo)
                    x = lastX;
                else if (y == 0 && segment.command == Command::HorizontalLineTo)
                    y = lastY;
                lastX = x;
                lastY = y;
            } else {
                segList[i].toAbsolute(lastX, lastY);
                lastX += segment.x;
                lastY += segment.y;
            }
        }
    }
}

void Path::segmentsToRelative(QList<Segment> &segList)
{
    qreal lastX = 0;
    qreal lastY = 0;
    for (int i = 0; i < segList.count(); ++i) {
        Segment segment = segList.at(i);
        if (segment.command != Command::ClosePath) {
            if (segment.absolute) {
                segList[i].toRelative(lastX, lastY);
                segment = segList.at(i);
                lastX += segment.x;
                lastY += segment.y;
            } else {
                lastX = segment.x;
                lastY = segment.y;
            }
        }
    }
}

QString Path::findAttribute(const QString &attrName)
{
    if (m_elem.isNull())
        return "";
    SvgElement parent = m_elem;
    while (!parent.isNull()) {
        if (parent.hasAttribute(attrName))
            return parent.attribute(attrName);
        parent = parent.parentNode();
    }
    return "";
}

void Path::processAbsoluteSegments(QList<Segment> &segList)
{
    if (!Keys::get().flag(Key::KeepUnusedSymbolsFromPath)) {
        // remove 'z' command if start point equal to last
        // except 'stroke-linejoin' is not default, because it causes render error
        QString strokeLinejoin = findAttribute("stroke-linejoin");
        if (strokeLinejoin.isEmpty() || strokeLinejoin == "miter") {
            QPointF prevM(segList.first().x, segList.first().y);
            for (int i = 1; i < segList.size(); ++i) {
                Segment prevSeg = segList.at(i-1);
                if (segList.at(i).command == Command::ClosePath) {
                    if (isZero(prevM.x() - prevSeg.x) && isZero(prevM.y() - prevSeg.y))
                        segList.removeAt(i--);
                    if (i != segList.size()-1)
                        prevM = QPointF(segList.at(i+1).x, segList.at(i+1).y);
                }
            }
        }
    }

    if (!Keys::get().flag(Key::KeepTinySegments)) {
        // remove tiny segments
        static qreal minValue = 1 / pow(10, Keys::get().coordinatesPrecision());
        for (int i = 0; i < segList.count()-1; ++i) {
            Segment seg = segList.at(i);
            Segment nextSeg = segList.at(i+1);
            if (seg.command == nextSeg.command && seg.command != Command::ClosePath
                    && seg.command != Command::EllipticalArc) {
                if (qAbs(nextSeg.x - seg.x) < minValue && qAbs(nextSeg.y - seg.y) < minValue) {
                    segList.removeAt(i+1);
                    i--;
                }
            }
        }
    }
}

// TODO: add other commands conv
void Path::processRelativeSegments(QList<Segment> &segList)
{
    if (!Keys::get().flag(Key::KeepTinySegments)) {
        for (int i = 0; i < segList.count(); ++i) {
            Segment seg = segList.at(i);
            const QString cmd = seg.command;

            // remove empty segments 'm0,0' (except first)
            if (cmd == Command::MoveTo && i > 0) {
                if (isZero(seg.x) && isZero(seg.y))
                    segList.removeAt(i--);
            }
            else if (cmd == Command::CurveTo) {
                if (   isZero(seg.x1) && isZero(seg.y1)
                    && isZero(seg.x2) && isZero(seg.y2)
                    && isZero(seg.x) && isZero(seg.y))
                {
                    segList.removeAt(i--);
                }
            }
            else if (   cmd == Command::LineTo
                     || cmd == Command::SmoothQuadratic) {
                if (isZero(seg.x) && isZero(seg.y))
                    segList.removeAt(i--);
            }
            else if (cmd == Command::HorizontalLineTo) {
                if (isZero(seg.x))
                    segList.removeAt(i--);
            }
            else if (cmd == Command::VerticalLineTo) {
                if (isZero(seg.y))
                    segList.removeAt(i--);
            }
            else if (cmd == Command::SmoothCurveTo) {
                if (   (isZero(seg.x2) && isZero(seg.y2))
                    && (isZero(seg.x) && isZero(seg.y)))
                    segList.removeAt(i--);
            }
            else if (cmd == Command::Quadratic) {
                if (   isZero(seg.x1) && isZero(seg.y1)
                    && isZero(seg.x) && isZero(seg.y))
                    segList.removeAt(i--);
            }
            else if (cmd == Command::EllipticalArc) {
                if (isZero(seg.x) && isZero(seg.y))
                    segList.removeAt(i--);
            }
        }
    }

    QList<Segment> newSegList;
    if (!Keys::get().flag(Key::KeepOriginalCurveTo)) {
        for (int i = 0; i < segList.count(); ++i) {
            Segment seg = segList.at(i);
            QString currCmd = seg.command;
            if (currCmd == Command::LineTo) {
                if (isZero(seg.x) && !isZero(seg.y)) {
                    seg.command = Command::VerticalLineTo;
                    newSegList << seg;
                } else if (!isZero(seg.x) && isZero(seg.y)) {
                    seg.command = Command::HorizontalLineTo;
                    newSegList << seg;
                } else {
                    newSegList << seg;
                }
            } else if (currCmd == Command::CurveTo) {
                if (isZero(seg.x1) && isZero(seg.y1) && isZero(seg.x2) && isZero(seg.x)
                        && qFuzzyCompare(seg.y2, seg.y)) {
                    seg.command = Command::VerticalLineTo;
                    newSegList << seg;
                } else if (isZero(seg.x1) && isZero(seg.y1) && isZero(seg.y2) && isZero(seg.y)
                           && qFuzzyCompare(seg.x2, seg.x)) {
                    seg.command = Command::HorizontalLineTo;
                    newSegList << seg;
                } else if (isZero(seg.x1) && isZero(seg.y1)
                           && qFuzzyCompare(seg.y2, seg.y) && qFuzzyCompare(seg.x2, seg.x)) {
                    seg.command = Command::LineTo;
                    newSegList << seg;
                } else if (i > 0) {
                    Segment prevSeg = segList.at(i-1);
                    if (   (qFuzzyCompare(seg.x1, (prevSeg.x - prevSeg.x2))
                            || (isZero(seg.x1) && isZero(prevSeg.x - prevSeg.x2)))
                        && (qFuzzyCompare(seg.y1, (prevSeg.y - prevSeg.y2))
                            || (isZero(seg.y1) && isZero(prevSeg.y - prevSeg.y2)))) {
                        seg.command = Command::SmoothCurveTo;
                        newSegList << seg;
                    } else if (isZero(seg.x1) && isZero(seg.y1) && prevSeg.command == Command::MoveTo) {
                        seg.command = Command::SmoothCurveTo;
                        newSegList << seg;
                    } else {
                        newSegList << seg;
                    }
                } else {
                    newSegList << seg;
                }
            } else {
                newSegList << seg;
            }
        }
    }
    segList = newSegList;
}

bool Path::isZero(double value)
{
    return Tools::isZero(value);
}

QString Path::segmentsToPath(QList<Segment> &segList)
{
    if (segList.size() == 1 || segList.isEmpty())
        return "";

    // rounding to >=4 decimal usually do not cause deformation
    bool isUseBiggerPrecisoin = false;
    const int smallPathPrecision = Keys::get().coordinatesPrecision() + 1;
    if (Keys::get().coordinatesPrecision() < 4) {
        // simple check to detect path with small segments wich can be damaged after rounding
        int smallNumCount = 0;
        for (int i = 0; i < segList.size(); ++i) {
            // FIXME: work only with relative
            if (qAbs(segList.at(i).x) < 1 /*|| qAbs(segList.at(i).y) < 1*/) {
                smallNumCount++;
                if (smallNumCount > segList.size()/2) {
                    isUseBiggerPrecisoin = true;
                    break;
                }
            }
        }
    }

    QString outPath;
    QChar prevCom;
    bool isPrevComAbs = false;
    bool isTrim = !Keys::get().flag(Key::KeepUnusedSymbolsFromPath);
    bool isApplyRound = !Keys::get().flag(Key::SkipRoundingNumbers);
    QString prevPos;
    for (int i = 0; i < segList.count(); ++i) {
        Segment segment = segList.at(i);
        QChar cmd = segment.command;
        // check is previous command is the same as next
        bool writeCmd = true;
        if (cmd == prevCom && !prevCom.isNull()) {
            if (segment.absolute == isPrevComAbs
                && !(segment.srcCmd && cmd == Command::MoveTo))
                writeCmd = false;
        }

        if (writeCmd) {
            if (segment.absolute)
                outPath += segment.command.toUpper();
            else
                outPath += segment.command;
            if (!isTrim)
                outPath += " ";
        }

        QVector<qreal> points;
        segment.coords(points);
        if (isTrim) {
            // remove unneeded number separators
            // m 30     to  m30
            // 10,-30   to  10-30
            // 15.1,.5  to  15.1.5
            bool round = isApplyRound;
            if (i+1 < segList.size() && round) {
                if ((cmd == Command::MoveTo || cmd == Command::EllipticalArc)
                    && segList.at(i+1).command == Command::EllipticalArc)
                    round = false;
            }
            if (cmd == Command::EllipticalArc)
               round = false;
            for (int j = 0; j < points.size(); ++j) {
                QString currPos;
                if (round) {
                    if (!isUseBiggerPrecisoin)
                        currPos = Tools::roundNumber(points.at(j));
                    else
                        currPos = Tools::roundNumber(points.at(j), smallPathPrecision);
                } else
                    currPos = Tools::roundNumber(points.at(j), 6);

                bool useSep = false;
                if (cmd == Command::EllipticalArc)
                    useSep = true;
                else if (!prevPos.contains('.') && currPos.startsWith('.'))
                    useSep = true;
                else if (currPos.at(0).isDigit())
                    useSep = true;

                if (j == 0 && writeCmd)
                    useSep = false;

                if (useSep)
                    outPath += " ";
                outPath += currPos;
                prevPos = currPos;
            }
        } else {
            for (int j = 0; j < points.size(); ++j) {
                if (isApplyRound)
                    outPath += Tools::roundNumber(points.at(j));
                else
                    outPath += QString::number(points.at(j));
            }
            if (cmd != Command::ClosePath)
                outPath += " ";
        }
        prevCom = cmd;
        isPrevComAbs = segment.absolute;
    }
    if (!isTrim)
        outPath.chop(1);
    return outPath;
}
