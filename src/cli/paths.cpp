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
    m_isApplyRound = !Keys::get().flag(Key::SkipRoundingNumbers);
}

QString Segment::string(qreal value) const
{
    return (m_isApplyRound) ? Tools::roundNumber(value) : QString::number(value);
}

void Segment::setTransform(const QString &text)
{
    Transform tr(text);
    if (command == Command::MoveTo || command == Command::LineTo
        || command == Command::SmoothQuadratic) {
        tr.setOldXY(x, y);
        x = tr.newX();
        y = tr.newY();
    } else if (command == Command::CurveTo) {
        tr.setOldXY(x, y);
        x = tr.newX();
        y = tr.newY();

        tr.setOldXY(x1, y1);
        x1 = tr.newX();
        y1 = tr.newY();

        tr.setOldXY(x2, y2);
        x2 = tr.newX();
        y2 = tr.newY();
    } else if (command == Command::SmoothCurveTo) {
        tr.setOldXY(x, y);
        x = tr.newX();
        y = tr.newY();

        tr.setOldXY(x2, y2);
        x2 = tr.newX();
        y2 = tr.newY();
    } else if (command == Command::Quadratic) {
        tr.setOldXY(x, y);
        x = tr.newX();
        y = tr.newY();

        tr.setOldXY(x1, y1);
        x1 = tr.newX();
        y1 = tr.newY();
    }
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

QStringList Segment::toStringList() const
{
    QStringList list;
    if (command == Command::CurveTo)
    {
        list.reserve(11);
        list << string(x1) << "," << string(y1) << " "
             << string(x2) << "," << string(y2) << " "
             << string(x)  << "," << string(y);
    }
    else if (command == Command::MoveTo || command == Command::LineTo
        || command == Command::SmoothQuadratic)
    {
        list.reserve(3);
        list << string(x) << "," << string(y);
    }
    else if (command == Command::HorizontalLineTo)
    {
        list.reserve(1);
        list << string(x);
    }
    else if (command == Command::VerticalLineTo)
    {
        list.reserve(1);
        list << string(y);
    }
    else if (command == Command::SmoothCurveTo)
    {
        list.reserve(7);
        list << string(x2) << "," << string(y2) << " "
             << string(x)  << "," << string(y);
    }
    else if (command == Command::Quadratic)
    {
        list.reserve(7);
        list << string(x1) << "," << string(y1) << " "
             << string(x)  << "," << string(y);
    }
    else if (command == Command::EllipticalArc)
    {
        list.reserve(13);
        list << string(rx) << "," << string(ry) << " "
             << QString::number(xAxisRotation) << " "
             << QString::number(largeArc) << ","
             << QString::number(sweep) << " "
             << string(x)  << "," << string(y);
    }
    return list;
}

// New class

void Path::processPath(SvgElement elem)
{
    const QString inPath = elem.attribute("d");

    if (inPath.contains("nan")) {
        elem.removeAttribute("d");
        return;
    }

    // TODO: path like 'M x,y A rx,ry 0 1 1 x,y', where x and y of both segments are identical
    // can't be converted to relative coordinates
    // some kind of bug
    if (inPath.contains("A")) {
        if (QString(inPath).remove(QRegExp("[^a-AZ-z]")) == "MA") {
            return;
        }
    }

    QList<Segment> segList = splitToSegments(inPath);

    // NOTE: didn't work, in some cases, without segmentsToRelative()
    segmentsToAbsolute(segList);
    segmentsToRelative(segList);
    processSegments(segList);

    // merge segments to path
    QString outPath = segmentsToPath(segList);

    // set old path, if new is longer
    if (outPath.size() > inPath.size())
        elem.setAttribute("d", inPath);
    else
        elem.setAttribute("d", outPath);
}

QList<Segment> Path::splitToSegments(const QString &path)
{
    QStringList data;
    QString buf;
    QChar prevNonDigit;
    // split path to commands and points
    int pathSize = path.size();
    for (int i = 0; i < pathSize; ++i) {
        QChar c = path.at(i);
        if (c.isLetter() && c.toLower() != 'e') {
            if (!buf.isEmpty())
                data << buf;
            data << c;
            buf.clear();
        } else if (c == ',' || c == ' ') {
            if (!buf.isEmpty())
                data << buf;
            buf.clear();
        } else if (i == pathSize-1) {
            buf += c;
            if (!buf.isEmpty())
                data << buf;
            buf.clear();
        } else {
            if (c.isNumber() || (c == '.' && prevNonDigit != '.') || c == 'e') {
                buf += c;
            } else if (c == '-' || c == '.') {
                if (path.at(i-1).isNumber()) {
                    if (!buf.isEmpty())
                        data << buf;
                    buf.clear();
                    buf += c;
                } else {
                    buf += c;
                }
            }
        }
        if (!c.isDigit())
            prevNonDigit = c;
    }

    // convert to segments
    QList<Segment> segList;
    QString currCmd;
    while (!data.isEmpty()) {
        bool isNewCmd = false;
        if (data.first().at(0).isLetter()) {
            currCmd = data.takeFirst();
            isNewCmd = true;
        }

        QChar lowerCmd = currCmd.at(0).toLower();
        Segment segment;

        segment.command = lowerCmd;
        segment.absolute = currCmd.at(0).isUpper();
        segment.srcCmd = isNewCmd;

        if (   lowerCmd == Command::MoveTo
            || lowerCmd == Command::LineTo
            || lowerCmd == Command::SmoothQuadratic) {
            while (data.size() < 2)
                data << "0";
            segment.x  = data.takeFirst().toDouble();
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::HorizontalLineTo) {
            while (data.size() < 1)
                data << "0";
            segment.x  = data.takeFirst().toDouble();
            segment.y  = 0;
        } else if (lowerCmd == Command::VerticalLineTo) {
            while (data.size() < 1)
                data << "0";
            segment.x  = 0;
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::CurveTo) {
            while (data.size() < 6)
                data << "0";
            segment.x1 = data.takeFirst().toDouble();
            segment.y1 = data.takeFirst().toDouble();
            segment.x2 = data.takeFirst().toDouble();
            segment.y2 = data.takeFirst().toDouble();
            segment.x  = data.takeFirst().toDouble();
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::SmoothCurveTo) {
            while (data.size() < 4)
                data << "0";
            segment.x2 = data.takeFirst().toDouble();
            segment.y2 = data.takeFirst().toDouble();
            segment.x  = data.takeFirst().toDouble();
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::Quadratic) {
            while (data.size() < 4)
                data << "0";
            segment.x1 = data.takeFirst().toDouble();
            segment.y1 = data.takeFirst().toDouble();
            segment.x  = data.takeFirst().toDouble();
            segment.y  = data.takeFirst().toDouble();
        } else if (lowerCmd == Command::EllipticalArc) {
            while (data.size() < 7)
                data << "0";
            segment.rx = data.takeFirst().toDouble();
            segment.ry = data.takeFirst().toDouble();
            segment.xAxisRotation = data.takeFirst().toDouble();
            segment.largeArc      = data.takeFirst().toDouble();
            segment.sweep         = data.takeFirst().toDouble();
            segment.x = data.takeFirst().toDouble();
            segment.y = data.takeFirst().toDouble();
        }

        segList.append(segment);
    }
    return segList;
}

// TODO: if last point equal to first - convert segment into z
void Path::processSegments(QList<Segment> &segList)
{
    // NOTE: work only with relative segments
    // TODO: add other commands conv

    if (!Keys::get().flag(Key::KeepEmptySegments)) {
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
    return (fabs(value) < 0.00001);
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

QString Path::segmentsToPath(QList<Segment> &segList)
{
    QString outPath;
    QChar prevCom;
    bool isPrevComAbs = false;
    bool isTrim = !Keys::get().flag(Key::KeepUnusedSymbolsFromPath);
    QString prevSeg;
    for (int i = 0; i < segList.count(); ++i) {
        Segment segment = segList.at(i);
        QChar currCmd = segment.command;
        // check is previous command is the same as next
        bool writeCmd = true;
        if (currCmd == prevCom && !prevCom.isNull()) {
            if (segment.absolute == isPrevComAbs
                && !(segment.srcCmd && segment.command == Command::MoveTo))
                writeCmd = false;
        }
        prevCom = currCmd;
        isPrevComAbs = segment.absolute;

        if (writeCmd) {
            if (segment.absolute)
                outPath += segment.command.toUpper();
            else
                outPath += segment.command;
            if (!isTrim)
                outPath += " ";
        }

        QStringList tmpList = segment.toStringList();
        if (isTrim) {
            // remove unneeded number separators
            // m 30     to  m30
            // 10,-30   to  10-30
            // 15.1,.5  to  15.1.5
            QString separator = " ";
            for (int j = 0; j < tmpList.size(); ++j) {
                QString currSeg = tmpList.at(j);
                if (j % 2 != 0) {
                    separator = currSeg;
                } else {
                    bool useSep = false;
                    if (currCmd == Command::EllipticalArc)
                        useSep = true;
                    else if (!prevSeg.contains('.') && currSeg.startsWith('.'))
                        useSep = true;
                    else if (currSeg.at(0).isDigit())
                        useSep = true;

                    if (j == 0 && writeCmd)
                        useSep = false;

                    if (useSep)
                        outPath += separator;
                    outPath += currSeg;
                    prevSeg = currSeg;
                }
            }
        } else {
            outPath += tmpList.join("");
            if (currCmd != Command::ClosePath)
                outPath += " ";
        }
    }
    if (!isTrim)
        outPath.chop(1);
    return outPath;
}
