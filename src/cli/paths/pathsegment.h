#ifndef PATHSEGMENT_H
#define PATHSEGMENT_H

#include <QVarLengthArray>

#include "../transform.h"

namespace Command {
    extern const QChar MoveTo;
    extern const QChar LineTo;
    extern const QChar HorizontalLineTo;
    extern const QChar VerticalLineTo;
    extern const QChar CurveTo;
    extern const QChar SmoothCurveTo;
    extern const QChar Quadratic;
    extern const QChar SmoothQuadratic;
    extern const QChar EllipticalArc;
    extern const QChar ClosePath;
}

struct ArcStruct
{
    double x1;
    double y1;
    double rx;
    double ry;
    double angle;
    bool large_arc_flag;
    bool sweep_flag;
    double x2;
    double y2;
    QList<double> recursive;
};

// TODO: split to subclasses
class PathSegment
{
public:
    PathSegment();
    void setTransform(Transform &ts);
    void toRelative(double xLast, double yLast);
    void coords(QVarLengthArray<double, 6> &points);
    QVarLengthArray<PathSegment, 3> toCurve(const PathSegment &prevSeg, const PathSegment &prevSeg2) const;

    QChar command;
    // stores original value
    bool absolute;
    // is this command defined in source path
    bool srcCmd;
    double x;
    double y;
    double x1;
    double y1;
    double x2;
    double y2;
    double rx;
    double ry;
    double xAxisRotation;
    bool largeArc;
    bool sweep;

private:
    QPointF rotatePoint(double x, double y, double rad) const;
    QVarLengthArray<QPointF, 9> arcToCurve(ArcStruct arc) const;
};

QDebug operator<<(QDebug dbg, const PathSegment &s);

#endif // PATHSEGMENT_H
