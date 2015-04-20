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

#include "../transform.h"
#include "../svgdom/svgdom.h"
#include "pathsegment.h"

class Path
{
public:
    explicit Path() {}
    void processPath(SvgElement elem, bool canApplyTransform, bool *isTransformApplyed);
    QString segmentsToPath(const PathSegmentList &segList) const;
    PathSegmentList pathToSegments(const QString &path) const;

private:
    SvgElement m_elem;

    void splitToSegments(const QString &path, PathSegmentList &segList) const;
    void processSegments(PathSegmentList &segList);
    void segmentsToRelative(PathSegmentList &segList, bool onlyIfSourceWasRelative);
    void calcNewStrokeWidth(const double scaleFactor);
    void applyTransform(PathSegmentList &tsSegList);
    bool isTsPathShorter();
    void fixRelative(PathSegmentList &segList);

    // segments processing
    bool removeZSegments(PathSegmentList &segList);
    bool removeUnneededMoveToSegments(PathSegmentList &segList);
    bool removeTinySegments(PathSegmentList &segList);
    bool convertSegments(PathSegmentList &segList);
    bool joinSegments(PathSegmentList &segList);
    void removeStrokeLinecap(PathSegmentList &segList);
};

#endif // PATHS_H
