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

#ifndef PATHS_H
#define PATHS_H

#include <QStringList>
#include <QPointF>

#include "tools.h"

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

class Segment
{
public:
    Segment();
    QStringList toStringList() const;
    QString string(qreal value) const;
    void setTransform(const QString &text);
    void toRelative(qreal xLast, qreal yLast);
    void toAbsolute(qreal xLast, qreal yLast);
    QList<Segment> toCurve(qreal prevX, qreal prevY);

    QChar command;
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
    int xAxisRotation;
    int largeArc;
    int sweep;

private:
    bool m_isApplyRound;
};

class Path
{
public:
    explicit Path() {}
    void processPath(SvgElement elem);
    QString segmentsToPath(QList<Segment> &segList);

private:
    QList<Segment> splitToSegments(const QString &path);
    void processSegments(QList<Segment> &segList);
    void segmentsToAbsolute(QList<Segment> &segList);
    void segmentsToRelative(QList<Segment> &segList);
    QString trimPath(const QString &path);
    bool isZero(double value);
};

#endif // PATHS_H
