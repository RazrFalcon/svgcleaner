#ifndef PATHS_H
#define PATHS_H

#include <QtCore/QStringList>
#include <QtCore/QPointF>

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

class Segment
{
public:
    Segment();
    QString toString() const;
    QString string(qreal value) const;
    void setTransform(const QString &text);
    void toRelative(qreal xLast, qreal yLast);
    void toAbsolute(qreal xLast, qreal yLast);
    QList<Segment> toCurve(qreal prevX, qreal prevY);


    QString command;
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
    void setSegments(const QList<Segment> &list);
    QString segmentsToPath();
    void segmentsToRelative();

private:
    QList<Segment> m_segmentList;
    void splitToSegments(const QString &path);
    void processSegments();
    void segmentsToAbsolute();
    QString trimPath(const QString &path);
};

#endif // PATHS_H
