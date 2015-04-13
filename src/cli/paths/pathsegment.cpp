#include <QtDebug>

#include <cmath>

#include "../mindef.h"
#include "../tools.h"
#include "pathsegment.h"

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

PathSegment::PathSegment()
{
    absolute = false;
    srcCmd   = false;
    largeArc = false;
    sweep    = false;
}

void PathSegment::setTransform(Transform &ts)
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

QPointF PathSegment::rotatePoint(double x, double y, double rad) const
{
    double X = x * cos(rad) - y * sin(rad);
    double Y = x * sin(rad) + y * cos(rad);
    return QPointF(X, Y);
}

// arcToCurve is a port from Raphael JavaScript library to Qt (MIT license)
QVarLengthArray<QPointF, 9> PathSegment::arcToCurve(ArcStruct arc) const
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

PathSegment quadToCurve(double x0, double y0, double x1, double y1, double x2, double y2)
{
    static const double v1 = 1.0 / 3.0;
    static const double v2 = 2.0 / 3.0;

    PathSegment seg;
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

PathSegment lineToCurve(double x1, double y1, double x2, double y2)
{
    PathSegment seg;
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

QVarLengthArray<PathSegment, 3> PathSegment::toCurve(const PathSegment &prevSeg, const PathSegment &prevSeg2) const
{
    QVarLengthArray<PathSegment, 3> segList;
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
        PathSegment seg;
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
        PathSegment seg = prevSeg2;
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
                PathSegment seg;
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

void PathSegment::toRelative(double xLast, double yLast)
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

void PathSegment::coords(QVarLengthArray<double, 6> &points)
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

QDebug operator<<(QDebug dbg, const PathSegment &s)
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
