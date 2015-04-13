#ifndef PATHBBOX_H
#define PATHBBOX_H

#include <QString>

#include "pathsegment.h"

class PathBBox
{
public:
    PathBBox() {}
    static QString calcBoundingBox(const QList<PathSegment> &segList, bool convertToCurves);
};

#endif // PATHBBOX_H
