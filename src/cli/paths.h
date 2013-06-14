#ifndef PATHS_H
#define PATHS_H

#include <QStringList>
#include <QPointF>

#include "tools.h"

namespace Command {
    static const QString MoveTo           = "m";
    static const QString LineTo           = "l";
    static const QString HorizontalLineTo = "h";
    static const QString VerticalLineTo   = "v";
    static const QString CurveTo          = "c";
    static const QString SmoothCurveTo    = "s";
    static const QString Quadratic        = "q";
    static const QString SmoothQuadratic  = "t";
    static const QString EllipticalArc    = "a";
    static const QString ClosePath        = "z";
}

struct Segment
{
    QString command;
    // absolute
    bool abs;
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
};

class SegmentList
{
public:
    SegmentList();
    bool next();
    int position();
    QString genPath();
    QString string(qreal value);
    void append(Segment data);
    void appendLastPoint(qreal x, qreal y);
    QPointF lastPoint();
    void removeCurrent();
    void restart();
    void setLastPoint(qreal x, qreal y);
//    void setTransform(const QString &text);
    void toRelative();
    void toAbsolute();
    Segment segment();
    void updateSegment(const Segment &seg);

private:
    int m_position;
    bool m_isRound;
    QPointF m_lastPoint;
    QList<Segment> m_data;
};

class Path
{
public:
    explicit Path() {}
    void processPath(SvgElement elem);
    void setSegments(const SegmentList &list);
    QString segmentsToPath();
    void segmentsToRelative();

private:
    SegmentList m_segmentList;
    void splitToSegments(const QString &path);
    void processSegments();
    void segmentsToAbsolute();
    QString trimPath(const QString &path);
};

#endif // PATHS_H
